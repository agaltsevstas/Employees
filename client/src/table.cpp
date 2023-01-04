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


namespace Client
{
    Table::Table(Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table),
        _requester(iRequester)
    {
        _ui->setupUi(this);
        _personalDataModel = new QJsonTableModel(_requester->getJson(), this);

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

        auto findTable = [&json](const QString& iTable)->int
        {
            for (decltype(json.array().size()) i = 0, I = json.array().size(); i < I; ++i)
            {
                if (json.array()[i].isObject())
                {
                    const QJsonObject subobject = json.array()[i].toObject();
                    if (subobject.contains(iTable))
                    {
                        return i;
                    }
                }
            }

            return -1;
        };

        auto index_data = findTable("employee");
        auto index_personal_permissions = findTable("personal_data_permission");
        auto index_permissions = findTable("permission");
        if (index_data != -1 &&
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

                    auto it_Data = subobject_data.find(field);
                    auto it_Permissions = subobject_permissions.find(field);
                    if (it_Data != subobject_data.end() && it_Permissions != subobject_permissions.end())
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

                        if (it_Data->isString())
                        {
                            lineEdit->setText(it_Data.value().toString());
                        }
                        else if (it_Data->isDouble())
                        {
                            lineEdit->setText(QString::number(it_Data.value().toInteger()));
                        }
                        else
                        {
                            Q_ASSERT(false);
                        }

                        if (it_Permissions->isString())
                        {
                            lineEdit->setEnabled(it_Permissions->toString() == "write");
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

    void Table::showDB(bool iResult)
    {
        if (iResult)
        {
            qDebug() << "Ответ на запрос получен!";
            if (!_tableView)
            {
                _tableView = new TableView(this);
            }

            _databaseModel = new QJsonTableModel(_requester->getJson(), this);
            _tableView->setModel(_databaseModel);
            _ui->splitter->addWidget(_tableView);
//            _tableView->horizontalHeader()->setSortIndicator(0, 0);
//            _ui->splitter->adjustSize();
        }
        else
        {
            qDebug() << "Ответ на запрос не получен!";
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
                handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1);
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

    void Table::on_update_clicked()
    {
        Requester::HandleResponse handleResponse;
        handleResponse = [this](bool iResult)
        {
            if (iResult)
            {
                qDebug() << "Ваши данные успешно обновлены!";
                setPersonalData(_requester->getJson());
                if (_databaseModel)
                {
                    Requester::HandleResponse handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1);
                    _requester->sendRequest("showDatabase", handleResponse);
                }
            }
            else
            {
                qDebug() << "Ошибка обновления!";
            }
        };

        _requester->sendRequest("login", handleResponse);
    }
}
