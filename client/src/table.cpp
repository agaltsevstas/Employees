#include "table.h"
#include "table_p.h"
#include "client.h"
#include "requester.h"
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
#include <QSettings>
#include <QSizePolicy>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>


namespace Client
{
    auto findTableInJson(const QJsonDocument &iJson, const QString& iTable)->int
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

    Table::Table(QSettings *iSettings, Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table()),
        _stackedWidget(new QStackedWidget(_ui->groupBox)),
        _settings(iSettings),
        _requester(iRequester)
    {
        _ui->setupUi(this);
        setPersonalData(_requester->getJson());

        QSizePolicy sizePolicy = GetSizePolice();
        setSizePolicy(sizePolicy);

        _stackedWidget->setSizePolicy(sizePolicy);
        _stackedWidget->addWidget(_personalData);
        _stackedWidget->setCurrentWidget(_personalData);

        _ui->gridLayout->addWidget(_stackedWidget, 0, 0, 1, 1);

        loadSettings();
    }

    Table::~Table()
    {
        saveSettings();

        delete _ui;
    }

    void Table::loadSettings()
    {
        auto update = _settings->value("update");
        move(_settings->value("centerTable", qApp->primaryScreen()->availableGeometry().center()).toPoint());

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
        _settings->setValue("centerTable", geometry().center());
        if (QCheckBox* autoUpdate = _personalData->findChild<QCheckBox*>("autoUpdate"))
            _settings->setValue("update", autoUpdate->isChecked());
    }

    void Table::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);
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
                connect(_personalData, SIGNAL(sendRequest(const QByteArray&)), this, SLOT(updatePersonalData(const QByteArray&)));
            }
        }
        else
        {
            Q_ASSERT(false);
        }
    }

    QSizePolicy Table::GetSizePolice() noexcept
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        return sizePolicy;
    }

    void Table::onExitClicked()
    {
        close();           // Закрытие окна
        emit openDialog(); // Вызов главного окна
    }

    void Table::onAutoUpdateClicked(bool isChecked)
    {
        if (_personalData)
            _personalData->setEditStrategy(isChecked ? TablePrivate::EditStrategy::OnFieldChange : TablePrivate::EditStrategy::OnManualSubmit);

        if (_tableView)
            _tableView->setEditStrategy(isChecked ? TableView::EditStrategy::OnFieldChange : TableView::EditStrategy::OnManualSubmit);

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
                if (_tableView->getModel())
                {
                    Requester::HandleResponse handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1, std::placeholders::_2);
                    _requester->sendRequest("showDatabase", handleResponse);
                }
            }
            else
            {
                qDebug() << "Ошибка: " << error;
            }
        };

        _requester->sendRequest("showPersonalData", handleResponse);
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
                _tableView->setModel("employee", QJsonDocument(database.toArray()), QJsonDocument(database_permissions.toObject()));
                if (QCheckBox* autoUpdate = _personalData->findChild<QCheckBox*>("autoUpdate"))
                    _tableView->setEditStrategy(autoUpdate->isChecked() ? TableView::EditStrategy::OnFieldChange : TableView::EditStrategy::OnManualSubmit);
                _tableView->createData = std::bind(&Table::createData, this, std::placeholders::_1);
                _tableView->deleteData = std::bind(&Table::deleteData, this, std::placeholders::_1);
                _tableView->updateData = std::bind(&Table::updateData, this, std::placeholders::_1);
                connect(_tableView->selectionModel(),
                        SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                        SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));

                connect(_personalData, SIGNAL(sendValueSearch(const QString &)), _tableView, SLOT(valueSearchChanged(const QString &)));
                connect(_personalData, SIGNAL(sendClearSearch()), _tableView, SLOT(clearSearchChanged()));
                _ui->splitter->addWidget(_tableView);
    //            _tableView->horizontalHeader()->setSortIndicator(0, 0);
            }
        }
        else
        {
            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), error, QMessageBox::NoButton, this);
            QTimer::singleShot(1000, &warning, &QMessageBox::close);
            warning.exec();
            qDebug() << "Ошибка: " << error;
        }
    }

    void Table::showDatabase()
    {
        QPushButton *showDatabase = qobject_cast<QPushButton*>(sender());
        const bool isCheckable = showDatabase->isCheckable();

        if (QPushButton* createUser = _ui->groupBox->findChild<QPushButton*>("createUser"))
            createUser->setEnabled(!isCheckable);

        if (isCheckable)
        {
            if (QPushButton* deleteUser = _personalData->findChild<QPushButton*>("deleteUser"))
            {
                deleteUser->setEnabled(false);
            }

            showDatabase->setText("Показать базу данных");
            _tableView->setHidden(true);
            _tableView->setParent(NULL);
            adjustSize();
        }
        else
        {
            if (_tableView)
            {
                _tableView->setHidden(false);
                _ui->splitter->addWidget(_tableView);
                adjustSize();
            }
            else
            {
                Requester::HandleResponse handleResponse;
                handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1, std::placeholders::_2);
                _requester->sendRequest("showDatabase", handleResponse);
            }

            showDatabase->setText("Скрыть базу данных");
        }

        showDatabase->setCheckable(!isCheckable);
    }

    void Table::updatePersonalData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Ваши данные успешно обновлены!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updatePersonalData", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::createData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Пользователи успешно добавлены!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::POST, iData);
    }

    void Table::deleteData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Пользователи успешно удалены!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::DELETE, iData);
    }

    void Table::updateData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Поле базы данных успешно обновлено!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::onCreateUserClicked()
    {
        if (!_userData)
        {
            _userData = new TablePrivate(Employee::employeeTable(), this);
            connect(_tableView,
                    SIGNAL(getUserData(const QString &, const std::function<void(QWidget*)>&)),
                    _userData,
                    SLOT(sendUserData(const QString &, const std::function<void(QWidget*)>&)));
        }

        _stackedWidget->addWidget(_userData);
        _stackedWidget->setCurrentWidget(_userData);
    }

    void Table::onDeleteUserClicked()
    {
        if (!_tableView->deleteUser())
        {

        }

        if (QPushButton* deleteUser = _personalData->findChild<QPushButton*>("deleteUser"))
            deleteUser->setVisible(false);

        if (QPushButton* restoreUser = _personalData->findChild<QPushButton*>("restoreUser"))
            restoreUser->setVisible(true);
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
        }
    }
}
