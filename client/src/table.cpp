#include "client.h"
#include "personaldatatable.h"
#include "requester.h"
#include "table.h"
#include "tableView.h"
#include "ui_table.h"

#include <QSqlTableModel>
#include <QSqlQueryModel>
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
        _ui(new Ui::Table),
        _settings(iSettings),
        _requester(iRequester)
    {
        _ui->setupUi(this);

        _ui->gridLayout_6->removeWidget(_ui->autoUpdate);
        _ui->gridLayout_6->removeWidget(_ui->update);
        _ui->gridLayout_6->removeWidget(_ui->revert);
        _ui->gridLayout_6->removeWidget(_ui->exit);
        _ui->gridLayout_6->addWidget(_ui->revert, 1, 0, 1, 1);

        setPersonalData(_requester->getJson());

        _ui->gridLayout_6->addWidget(_ui->autoUpdate, 0, 0, 1, _ui->gridLayout_6->columnCount() / 2);
        _ui->gridLayout_6->addWidget(_ui->update, 0, 2, 1, _ui->gridLayout_6->columnCount() / 2);
        _ui->gridLayout_6->addWidget(_ui->exit, 2, 0, 1, _ui->gridLayout_6->columnCount());
        _ui->gridLayout_6->addWidget(new QProgressBar(_requester->getProgressBar()), 3, 0, 1, _ui->gridLayout_6->columnCount());

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
        _ui->autoUpdate->setChecked(update.isNull() ? true : update.toBool());
        move(_settings->value("centerTable", qApp->primaryScreen()->availableGeometry().center()).toPoint());
        on_autoUpdate_clicked(_ui->autoUpdate->isChecked());
    }

    void Table::saveSettings()
    {
        _settings->setValue("centerTable", geometry().center());
        _settings->setValue("update", _ui->update->isChecked());
    }

    void Table::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);
    }

    void Table::setPersonalData(const QJsonDocument &iJson)
    {
        if (!iJson.isArray())
        {
            Q_ASSERT(false);
            return;
        }

//        sizePolicy.setHeightForWidth(_ui->PersonalData->hasHeightForWidth());
        while (auto item = _ui->gridLayout_2->takeAt(0))
            delete item;

        _ui->personalData->adjustSize();
        _ui->gridLayout_2->update();

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
            if (data.isObject() &&
                personal_permissions.isObject() &&
                permissions.isObject())
            {
                _personalData = new PersonalDataTable(Employee::employeeTable(),
                                                      QJsonDocument(data.toObject()),
                                                      QJsonDocument(personal_permissions.toObject()),
                                                      _ui->gridLayout_2,
                                                      _ui->personalData,
                                                      this);
                _personalData->setEditStrategy(_ui->autoUpdate->isChecked() ? PersonalDataTable::EditStrategy::OnFieldChange : PersonalDataTable::EditStrategy::OnManualSubmit);

                connect(_personalData, SIGNAL(sendRequest(const QByteArray&)), this, SLOT(updatePersonalData(const QByteArray&)));

                const QJsonObject subobject_permissions = permissions.toObject(); // обязательно нужно определить
                auto show_db = subobject_permissions.find("show_db");
                auto create_user = subobject_permissions.find("create_user");
                auto delete_user = subobject_permissions.find("delete_user");
                if (show_db != subobject_permissions.end() &&
                    create_user != subobject_permissions.end() &&
                    delete_user != subobject_permissions.end())
                {
                    if (show_db->isBool() && show_db->toBool())
                    {
                        QPushButton *showDatabase = new QPushButton("Показать базу данных", this);
                        showDatabase->setSizePolicy(GetSizePolice());
                        connect(showDatabase, SIGNAL(clicked()), this, SLOT(showDatabase()));
                        _ui->gridLayout_6->addWidget(showDatabase, 1, 1, 1, 1);
                    }

                    if (create_user->isBool() && create_user->toBool())
                    {
                        QPushButton *createUser = new QPushButton("Создать пользователя", this);
                        connect(createUser, SIGNAL(clicked()), this, SLOT(onCreateUserClicked()));
                        createUser->setSizePolicy(GetSizePolice());
                        _ui->gridLayout_6->addWidget(createUser, 1, 2, 1, 1);
                    }

                    if (delete_user->isBool() && delete_user->toBool())
                    {
                        QPushButton *deleteUser = new QPushButton("Удалить пользователя", this);
                        connect(deleteUser, SIGNAL(clicked()), this, SLOT(onDeleteUserClicked()));
                        deleteUser->setSizePolicy(GetSizePolice());
                        _ui->gridLayout_6->addWidget(deleteUser, 1, 3, 1, 1);
                    }

                }
            }
        }
        else
        {
            Q_ASSERT(false);
        }

        _ui->personalData->adjustSize();
        _ui->gridLayout_2->update();
    }

    QSizePolicy Table::GetSizePolice()
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        return sizePolicy;
    }

    void Table::on_exit_clicked()
    {
        close();           // Закрытие окна
        emit openDialog(); // Вызов главного окна
    }

    void Table::on_autoUpdate_clicked(bool isChecked)
    {
        if (_personalData)
            _personalData->setEditStrategy(isChecked ? PersonalDataTable::EditStrategy::OnFieldChange : PersonalDataTable::EditStrategy::OnManualSubmit);
        if (_databaseModel)
            _databaseModel->setEditStrategy(isChecked ? QJsonTableModel::EditStrategy::OnFieldChange : QJsonTableModel::EditStrategy::OnManualSubmit);

        _ui->update->setVisible(!isChecked);
        _ui->update->setEnabled(!isChecked);
    }

    void Table::on_update_clicked()
    {
        if (_personalData)
            _personalData->submitAll();

        if (_databaseModel)
            _databaseModel->submitAll();
    }

    void Table::on_revert_clicked()
    {
        Requester::HandleResponse handleResponse;
        handleResponse = [this](bool iResult, const QString &error)
        {
            if (iResult)
            {
                qDebug() << "Ваши данные успешно обновлены!";
                setPersonalData(_requester->getJson());
                if (_databaseModel)
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

            if (!_tableView)
                _tableView = new TableView(this);

            const QJsonDocument json = _requester->getJson();
            auto index_database = findTableInJson(json, "employee");
            auto index_database_permissions = findTableInJson(json, "database_permission");
            if (index_database != -1 &&
                index_database_permissions != -1)
            {
                const QJsonValue database = json.array()[index_database].toObject().value("employee");
                const QJsonValue database_permissions = json.array()[index_database_permissions].toObject().value("database_permission");
                if (database.isArray() &&
                    database_permissions.isObject())
                {
                    _databaseModel = new QJsonTableModel("employee", QJsonDocument(database.toArray()), QJsonDocument(database_permissions.toObject()), this);
                    _databaseModel->setEditStrategy(_ui->autoUpdate->isChecked() ? QJsonTableModel::EditStrategy::OnFieldChange : QJsonTableModel::EditStrategy::OnManualSubmit);
                    connect(_databaseModel, SIGNAL(sendRequest(const QByteArray&)), this, SLOT(updateData(const QByteArray&)));
                    _tableView->setModel(_databaseModel);
                    _ui->splitter->addWidget(_tableView);
        //            _tableView->horizontalHeader()->setSortIndicator(0, 0);
        //            _ui->splitter->adjustSize();
                    return;
                }
            }

            qDebug() << "База данных сотрудников не найдена!";
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
//        QTableView* table = new QTableView();
//        table->setSelectionBehavior(QAbstractItemView::SelectRows);
//        table->setSelectionMode(QAbstractItemView::SingleSelection);
//        QItemSelectionModel *select = table->selectionModel();

//        select->hasSelection();
//        select->selectedRows();
//        select->selectedColumns();
        QPushButton *showDatabase = qobject_cast<QPushButton*>(sender());
        const bool isCheckable = showDatabase->isCheckable();
        if (!isCheckable)
        {
            if (_databaseModel)
            {
                _tableView->setHidden(false);
//                _ui->splitter->adjustSize();
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
        else
        {
            showDatabase->setText("Показать базу данных");
            _tableView->setHidden(true);
            adjustSize();
        }

        if (_tableView)
        {
//            auto del1 = _ui->groupBox->size();
//            auto del2 = _tableView->geometry().size();
//            auto del3 = del1 + del2;
        }

        showDatabase->setCheckable(!isCheckable);
    }

    void Table::updatePersonalData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse;
        handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Ваши данные успешно обновлены!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updatePersonalData", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::updateData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse;
        handleResponse = [](const bool iResult, const QString &error)
        {
            qDebug() << (iResult ? "Поле базы данных успешно обновлено!" : ("Ошибка: " + error));
        };

        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::PATCH, iData);
    }

    void Table::onCreateUserClicked()
    {
        if (!_databaseModel->createUser())
        {
//            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), error, QMessageBox::NoButton, this);
//            QTimer::singleShot(1000, &warning, &QMessageBox::close);
//            warning.exec();
        }
    }

    void Table::onDeleteUserClicked()
    {
        if (!_databaseModel->createUser())
        {

        }
    }
}
