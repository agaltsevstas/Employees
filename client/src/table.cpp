#include "table.h"
#include "ui_table.h"
#include "client.h"
#include "requester.h"
#include "tableView.h"

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QJsonTableModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSizePolicy>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QProgressBar>
#include <QMessageBox>
#include <QTimer>


namespace Client
{

    auto findTableInJson(const QJsonDocument &iJson, const QString& iTable)->int
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

        return -1;
    };

    Table::Table(Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table),
        _requester(iRequester)
    {
        _ui->setupUi(this);

        _ui->gridLayout_6->removeWidget(_ui->update);
        _ui->gridLayout_6->removeWidget(_ui->exit);
        _ui->gridLayout_6->addWidget(_ui->update, 0, 0, 1, 1);

        setPersonalData(_requester->getJson());

        _ui->gridLayout_6->addWidget(_ui->exit, 1, 0, 1, _ui->gridLayout_6->columnCount());
        _ui->gridLayout_6->addWidget(new QProgressBar(_requester->getProgressBar()), 2, 0, 1, _ui->gridLayout_6->columnCount());
    }

    void Table::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);
    }

    void Table::setPersonalData(const QJsonDocument &json)
    {
//        sizePolicy.setHeightForWidth(_ui->PersonalData->hasHeightForWidth());
        while (auto item = _ui->gridLayout_2->takeAt(0))
            delete item;

        _ui->PersonalData->adjustSize();
        _ui->gridLayout_2->update();

        auto index_data = findTableInJson(json, "employee");
        auto index_personal_permissions = findTableInJson(json, "personal_data_permission");
        auto index_permissions = findTableInJson(json, "permission");
        if (json.isArray() &&
            index_data != -1 &&
            index_personal_permissions != -1 &&
            index_permissions != -1)
        {
            const QJsonValue data = json.array()[index_data].toObject().value("employee");
            const QJsonValue personal_permissions = json.array()[index_personal_permissions].toObject().value("personal_data_permission");
            const QJsonValue permissions = json.array()[index_permissions].toObject().value("permission");
            if (data.isObject() &&
                personal_permissions.isObject() &&
                permissions.isObject())
            {
                const auto fields = Employee::getFileds();
                for (decltype(fields.size()) i = 0, I = fields.size(); i < I; ++i)
                {
                    const auto& [field, name] = fields[i];
                    const QJsonObject subobject_data = data.toObject(); // обязательно нужно определить
                    const QJsonObject subobject_permissions = personal_permissions.toObject(); // обязательно нужно определить

                    auto it_data = subobject_data.find(field);
                    auto it_permissions = subobject_permissions.find(field);
                    if (it_data != subobject_data.end() && it_permissions != subobject_permissions.end())
                    {
                        QLabel *label = new QLabel(field + "Label", _ui->PersonalData);
                        label->setSizePolicy(GetSizePolice());
                        label->setText(name);
                        _ui->gridLayout_2->addWidget(label, i, 0, 1, 1);

                        QLineEdit *lineEdit = new QLineEdit(field, _ui->PersonalData);
                        if (field == "password")
                        {
                            lineEdit->setEchoMode(QLineEdit::Password);
                        }

                        if (it_data->isString())
                        {
                            lineEdit->setText(it_data.value().toString());
                        }
                        else if (it_data->isDouble())
                        {
                            lineEdit->setText(QString::number(it_data.value().toInteger()));
                        }
                        else
                        {
                            Q_ASSERT(false);
                        }

                        if (it_permissions->isString())
                        {
                            lineEdit->setEnabled(it_permissions->toString() == "write");
                        }
                        else
                        {
                            Q_ASSERT(false);
                        }
    //                    sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
                        lineEdit->setSizePolicy(GetSizePolice());
                        _ui->gridLayout_2->addWidget(lineEdit, i, 1, 1, 1);
                    }
                }

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
                        _ui->gridLayout_6->addWidget(showDatabase, 0, 1, 1, 1);
                    }

                    if (create_user->isBool() && create_user->toBool())
                    {
                        QPushButton *createUser = new QPushButton("Создать пользователя", this);
                        createUser->setSizePolicy(GetSizePolice());
                        _ui->gridLayout_6->addWidget(createUser, 0, 2, 1, 1);
                    }

                    if (delete_user->isBool() && delete_user->toBool())
                    {
                        QPushButton *deleteUser = new QPushButton("Удалить пользователя", this);
                        deleteUser->setSizePolicy(GetSizePolice());
                        _ui->gridLayout_6->addWidget(deleteUser, 0, 3, 1, 1);
                    }

                }
            }
        }
        else
        {
            Q_ASSERT(false);
        }

        _ui->PersonalData->adjustSize();
        _ui->gridLayout_2->update();
    }

    QSizePolicy Table::GetSizePolice()
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        return sizePolicy;
    }

    Table::~Table()
    {
        delete _ui;
    }

    void Table::on_exit_clicked()
    {
        close();           // Закрытие окна
        emit openDialog(); // Вызов главного окна
    }

    void Table::on_update_clicked()
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

        _requester->sendRequest("showPersnalData", handleResponse);
    }

    void Table::showDB(bool iResult, const QString &error)
    {
        if (iResult)
        {
            qDebug() << "Ответ на запрос получен!";

            if (!_tableView)
                _tableView = new TableView(this);

            const QJsonDocument json = _requester->getJson();
            auto index_database = findTableInJson(json, "employee");
            auto index_database_permissions = findTableInJson(json, "database_permission");
            if (_requester->getJson().isArray() &&
                index_database != -1 &&
                index_database_permissions != -1)
            {
                const QJsonValue database = json.array()[index_database].toObject().value("employee");
                const QJsonValue database_permissions = json.array()[index_database_permissions].toObject().value("database_permission");
                if (database.isArray() &&
                    database_permissions.isObject())
                {
                    _databaseModel = new QJsonTableModel("employee", QJsonDocument(database.toArray()), QJsonDocument(database_permissions.toObject()), this);
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

    void Table::updateData(const QByteArray &iData)
    {
        Requester::HandleResponse handleResponse;
        handleResponse = [](bool iResult, const QString &error)
        {
            if (iResult)
            {
                qDebug() << "Ваши данные успешно обновлены!";
            }
            else
            {
                qDebug() << "Ошибка: " << error;
            }
        };

        _requester->sendRequest("updateDatabase", handleResponse, Requester::Type::PATCH, iData);
    }
}
