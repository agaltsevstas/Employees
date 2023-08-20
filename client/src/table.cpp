#include "table.h"
#include "table_p.h"
#include "client.h"
#include "requester.h"
#include "session.h"
#include "tableView.h"
#include "ui_table.h"

#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QJsonTableModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressBar>
#include <QScreen>
#include <QSizePolicy>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <Settings>


namespace Client
{
    [[nodiscard]] auto findTableInJson(const QJsonDocument &iJson, const QString& iTable)->int
    {
        if (iJson.isArray())
        {
            for (decltype(iJson.array().size()) i = 0, I = iJson.array().size(); i < I; ++i)
            {
                if (iJson.array()[i].isObject())
                {
                    const QJsonObject subobject = iJson.array()[i].toObject();
                    if (subobject.contains(iTable))
                    {
                        return i;
                    }
                }
            }
        }

        return -1;
    };

    Table::Table(Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table()),
        _stackedWidget(new QStackedWidget(_ui->groupBox)),
        _requester(iRequester)
    {
        _ui->setupUi(this);
        setObjectName("Table");
        setPersonalData(_requester->getJson());

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        setSizePolicy(sizePolicy);

        _stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        _stackedWidget->setSizePolicy(sizePolicy);
        _stackedWidget->addWidget(_personalData);
        _stackedWidget->setCurrentWidget(_personalData);

        _ui->gridLayout->addWidget(_stackedWidget, 0, 0, 1, 1);
        _ui->gridLayout->addWidget(_requester->getProgressBar(), 1, 0, 1, 1);

        connect(_requester, &Requester::logout, this, &Table::onExitClicked);
    }

    Table::~Table()
    {
        saveSettings();

        delete _ui;
    }

    void Table::loadSettings()
    {
        auto update = Session::getSession().Settings().value("update");
        move(Session::getSession().Settings().value("centerTable", qApp->primaryScreen()->availableGeometry().center()).toPoint());

        if (_personalData)
        {
            if (QCheckBox* autoUpdate = _personalData->findChild<QCheckBox*>("autoUpdate"))
            {
                autoUpdate->setChecked(update.isNull() ? true : update.toBool());
                onAutoUpdateClicked(autoUpdate->isChecked());
            }
        }
    }

    void Table::saveSettings()
    {
        Session::getSession().Settings().setValue("centerTable", geometry().center() - QPoint(width() / 2, height() / 2));
        if (QCheckBox* autoUpdate = _personalData->findChild<QCheckBox*>("autoUpdate"))
            Session::getSession().Settings().setValue("update", autoUpdate->isChecked());
    }

    void Table::selectionChanged(const QItemSelection &, const QItemSelection &)
    {
        QPushButton* deleteUser = _personalData->findChild<QPushButton*>("deleteUser");
        QPushButton* restoreUser = _personalData->findChild<QPushButton*>("restoreUser");
        if (!deleteUser || !restoreUser)
            return;

        auto canDelete = _tableView->canDelete();
        if (canDelete.has_value())
        {
            if (canDelete.value())
            {
                deleteUser->setEnabled(true);
                deleteUser->setVisible(true);
                restoreUser->setVisible(false);
            }
            else
            {
                restoreUser->setVisible(true);
                deleteUser->setEnabled(false);
                deleteUser->setVisible(false);
            }
        }
        else
        {
            deleteUser->setEnabled(false);
            deleteUser->setVisible(true);
            restoreUser->setVisible(false);
        }
    }

    void Table::setPersonalData(const QJsonDocument &iJson)
    {
        if (!iJson.isArray())
        {
            Q_ASSERT(false);
            return;
        }

        auto index_data = findTableInJson(iJson, "employee");
        auto index_personal_permissions = findTableInJson(iJson, "personal_data_permission");
        auto index_permissions = findTableInJson(iJson, "permission");
        if (index_data != -1 &&
            index_personal_permissions != -1 &&
            index_permissions != -1)
        {
            const QJsonValue data = iJson.array()[index_data].toObject().value("employee");
            const QJsonValue personal_permissions = iJson.array()[index_personal_permissions].toObject().value("personal_data_permission");
            const QJsonValue permissions = iJson.array()[index_permissions].toObject().value("permission");
            if (data.isObject() && personal_permissions.isObject() && permissions.isObject())
            {
                _personalData = new TablePrivate(Employee::employeeTable(),
                                                QJsonDocument(data.toObject()),
                                                QJsonDocument(personal_permissions.toObject()),
                                                QJsonDocument(permissions.toObject()),
                                                this);
                connect(_personalData, &TablePrivate::sendRequest, this, &Table::updatePersonalData);
                connect(_personalData, &TablePrivate::logout, this, &Table::onExitClicked);

                if (_stackedWidget && _stackedWidget->currentWidget() &&
                    _stackedWidget->currentWidget() != _personalData)
                {
                    auto currentWidget = _stackedWidget->currentWidget();
                    _stackedWidget->removeWidget(currentWidget);
                    _stackedWidget->addWidget(_personalData);
                    _stackedWidget->setCurrentWidget(_personalData);
                    delete currentWidget;
                }

                loadSettings();
            }
        }
        else
        {
            Q_ASSERT(false);
        }
    }

    bool Table::checkChanges() const noexcept
    {
        return (_personalData && _personalData->checkChanges()) || (_tableView && _tableView->checkChanges());
    }

    void Table::onExitClicked()
    {
        if (checkChanges())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Обновления", "Есть неотданные изменения, хотите выйти? (изменения не уйдут на сервер)", QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No)
                return;
        }

        _requester->getProgressBar()->setParent(NULL);
        showNormal();
        close();           // Закрытие окна
        emit openDialog(); // Вызов главного окна
    }

    void Table::onAutoUpdateClicked(bool isChecked)
    {
        /// Первым должно обновляться БД, потому в личных данных может быть смена роли
        if (_tableView)
        {
            _tableView->setEditStrategy(isChecked ? TableView::EditStrategy::OnFieldChange : TableView::EditStrategy::OnManualSubmit);
            _tableView->selectionModel()->clearSelection();
        }

        if (_personalData)
            _personalData->setEditStrategy(isChecked ? TablePrivate::EditStrategy::OnFieldChange : TablePrivate::EditStrategy::OnManualSubmit);

        if (QPushButton* update = _personalData->findChild<QPushButton*>("update"))
        {
            update->setVisible(!isChecked);
            update->setEnabled(!isChecked);
        }
    }

    void Table::onUpdateClicked()
    {
        if (_personalData)
            _personalData->submitAll();

        if (_tableView)
            _tableView->submitAll();
    }

    void Table::onRevertClicked()
    {
        Requester::HandleResponse handleResponse = [this](bool iResult, const QString &error)
        {
            if (iResult)
            {
                qDebug() << "Ваши данные успешно обновлены!";
                setPersonalData(_requester->getJson());
                if (_tableView && _tableView->getModel())
                {
                    Requester::HandleResponse handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1, std::placeholders::_2);
                    _requester->sendRequest("showDatabase", handleResponse);
                }
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), error, QMessageBox::NoButton, this);
                QTimer::singleShot(1000, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        };

        _requester->sendRequest("showPersonalData", handleResponse);
    }

    void Table::setEnabledDatabaseButtons(bool isEnable)
    {
        if (QPushButton* createUser = _ui->groupBox->findChild<QPushButton*>("createUser"))
            createUser->setEnabled(isEnable);

        if (QPushButton* deleteUser = _personalData->findChild<QPushButton*>("deleteUser"); deleteUser && deleteUser->isVisible())
            deleteUser->setEnabled(isEnable && _tableView && _tableView->selectionModel()->hasSelection());

        if (QPushButton* restoreUser = _personalData->findChild<QPushButton*>("restoreUser"); restoreUser && restoreUser->isVisible())
            restoreUser->setEnabled(isEnable && _tableView && _tableView->selectionModel()->hasSelection());

        if (QPushButton *search = _personalData->findChild<QPushButton*>("search"))
            search->setEnabled(isEnable);

        if (QLineEdit *valueSearch = _personalData->findChild<QLineEdit*>("valueSearch"))
            valueSearch->setEnabled(isEnable);
    }

    void Table::showDB(const bool iResult, const QString &error)
    {   
        if (iResult)
        {
            qDebug() << "Ответ на запрос получен!";

            if (!_requester->getJson().isArray())
            {
                Q_ASSERT(false);
                return;
            }

            const QJsonDocument json = _requester->getJson();
            auto index_database = findTableInJson(json, "employee");
            auto index_database_permissions = findTableInJson(json, "database_permission");
            if (index_database == -1 || index_database_permissions == -1)
            {
                qDebug() << "База данных сотрудников не найдена!";
                return;
            }

            const QJsonValue database = json.array()[index_database].toObject().value("employee");
            const QJsonValue database_permissions = json.array()[index_database_permissions].toObject().value("database_permission");
            if (!database.isArray() || !database_permissions.isObject())
            {
                qDebug() << "База данных сотрудников не найдена!";
                return;
            }

            if (!_tableView)
            {
                _tableView = new TableView(this);
                _ui->splitter->addWidget(_tableView);
            }
            else
            {
                _tableView->clearSearchChanged();
            }

            _tableView->setModel("employee", QJsonDocument(database.toArray()), QJsonDocument(database_permissions.toObject()));
            if (QCheckBox* autoUpdate = _personalData->findChild<QCheckBox*>("autoUpdate"))
                _tableView->setEditStrategy(autoUpdate->isChecked() ? TableView::EditStrategy::OnFieldChange : TableView::EditStrategy::OnManualSubmit);
            connect(_tableView, &TableView::sendCreateData, this, &Table::createData);
            connect(_tableView, &TableView::sendDeleteData, this, &Table::deleteData);
            connect(_tableView, &TableView::sendUpdateData, this, &Table::updateData);
            connect(_tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Table::selectionChanged);
            connect(_personalData, &TablePrivate::sendValueSearch, _tableView, &TableView::valueSearchChanged);
            connect(_personalData, &TablePrivate::sendClearSearch, _tableView, &TableView::clearSearchChanged);

            if (QPushButton *showDatabase = _ui->groupBox->findChild<QPushButton*>("showDatabase"))
                showDatabase->setText("Скрыть базу данных");

            showFullScreen();
        }
        else
        {
            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), error, QMessageBox::NoButton, this);
            QTimer::singleShot(1000, &warning, &QMessageBox::close);
            warning.exec();
            qDebug() << "Ошибка: " << error;
        }

        setEnabledDatabaseButtons(iResult);
    }

    void Table::showDatabase()
    {
        QPushButton *showDatabase = qobject_cast<QPushButton*>(sender());
        if (!showDatabase)
            return;

        const bool isCheckable = showDatabase->isCheckable();
        showDatabase->setCheckable(!isCheckable);

        if (isCheckable)
        {
            if (_tableView)
            {
                showDatabase->setText("Показать базу данных");
                setEnabledDatabaseButtons(false);
                _tableView->setHidden(true);
                _tableView->setParent(NULL);
                if (_tableView->getModel())
                    showNormal();
            }
        }
        else
        {
            if (_tableView)
            {
                showDatabase->setText("Скрыть базу данных");
                setEnabledDatabaseButtons(true);
                _tableView->setHidden(false);
                _ui->splitter->addWidget(_tableView);
                if (_tableView->getModel())
                    showFullScreen();
            }
            else
            {
                Requester::HandleResponse handleResponse;
                handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1, std::placeholders::_2);
                _requester->sendRequest("showDatabase", handleResponse);
            }
        }

    }

    void Table::updatePersonalData(const QByteArray &iData, const HandleResponse &handleResponse)
    {
        _requester->sendRequest("updatePersonalData", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::createData(const QByteArray &iData, const HandleResponse &handleResponse)
    {
        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::POST, iData);
    }

    void Table::deleteData(const QByteArray &iData, const HandleResponse &handleResponse)
    {
        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::DELETE, iData);
    }

    void Table::updateData(const QByteArray &iData, const HandleResponse &handleResponse)
    {
        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::onCreateUserClicked()
    {
        if (!_userData)
        {
            _userData = new TablePrivate("newEmployee", this);
            connect(_tableView, &TableView::getUserData, _userData, &TablePrivate::sendUserData);
        }

        _stackedWidget->addWidget(_userData);
        _stackedWidget->setCurrentWidget(_userData);
    }

    void Table::onDeleteUserClicked()
    {
        _tableView->deleteUser();
    }

    void Table::onRestoreUserClicked()
    {
        _tableView->restoreUser();

        if (QPushButton* deleteUser = _personalData->findChild<QPushButton*>("deleteUser"))
        {
            deleteUser->setEnabled(true);
            deleteUser->setVisible(true);
        }

        QPushButton* restoreUser = _personalData->findChild<QPushButton*>("restoreUser");
            restoreUser->setVisible(false);
    }

    void Table::onCancelClicked()
    {
        _stackedWidget->setCurrentWidget(_personalData);
    }

    void Table::onAddUserClicked()
    {
        if (_tableView->addUser())
        {
            _stackedWidget->setCurrentWidget(_personalData);
            _stackedWidget->removeWidget(_userData);
            delete _userData;
            _userData = nullptr;
        }
    }
}
