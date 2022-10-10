#include "table.h"
#include "ui_table.h"
#include "client.h"
#include "requester.h"

#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QJsonTableModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSizePolicy>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QSplitter>
#include <QHeaderView>


namespace Client
{
    Table::Table(Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table),
        _requester(iRequester)
    {
        _ui->setupUi(this);
        setPersonalData(_requester->getJson());
        _personalDataModel = new QJsonTableModel(_requester->getJson(), this);

        _ui->gridLayout->removeWidget(_ui->update);
        _ui->gridLayout->addWidget(_ui->update, 1, 0, 1, 1);

        QPushButton *showDatabase = new QPushButton("Показать базу данных", this);
        showDatabase->setSizePolicy(GetSizePolice());
        connect(showDatabase, SIGNAL(clicked()), this ,SLOT(showDatabase()));
        _ui->gridLayout->addWidget(showDatabase, 1, 1, 1, 1);
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

        for (const auto &array: json.array())
        {
            const QJsonObject object = array.toObject();
            auto fields = Employee::getFileds();
            auto fieldSize = fields.size();
            for (decltype(fieldSize) i = 0; i < fieldSize; ++i)
            {
                const auto& [field, name] = fields[i];
                if (auto it = object.find(field); it != object.end())
                {
                    QLabel *label = new QLabel(field + "Label", _ui->PersonalData);
                    label->setSizePolicy(GetSizePolice());
                    label->setText(name);
                    _ui->gridLayout_2->addWidget(label, i + 2, 0, 1, 1);

                    QLineEdit *lineEdit = new QLineEdit(field, _ui->PersonalData);
                    if (it->isString())
                    {
                        lineEdit->setText(it.value().toString());
                    }
                    else if (it->isDouble())
                    {
                        lineEdit->setText(QString::number(it.value().toInteger()));
                    }
                    else
                    {
                        Q_ASSERT(false);
                    }
//                    sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
                    lineEdit->setSizePolicy(GetSizePolice());
                    _ui->gridLayout_2->addWidget(lineEdit, i + 2, 1, 1, 1);
                }
            }
        }
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
        delete _personalDataModel;
        delete _databaseModel;
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
            delete _tableView;
            _tableView = new QTableView(this);
            _tableView->setObjectName(QString::fromUtf8("tableView"));
            auto sizePolicy = GetSizePolice();
            sizePolicy.setHorizontalStretch(10);
            _tableView->setSizePolicy(sizePolicy);
            /* Устанавливаем названия колонок в таблице с сортировкой данных
             * */
//            for(int i = 0, j = 0; i < model->columnCount(); i++, j++)
//            {
//                _table.setHeaderData(i,Qt::Horizontal,headers[j]);
//            }

//                        _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
            _databaseModel = new QJsonTableModel(_requester->getJson(), this);
            _tableView->setModel(_databaseModel);
            _tableView->resizeColumnsToContents();
            _tableView->adjustSize();
            _ui->splitter->addWidget(_tableView);
            _ui->splitter->adjustSize();
//                        resize(_ui->groupBox->size() + _tableView->geometry().size());
//                        _ui->groupBox->adjustSize();
//                        adjustSize();
//            resize(_width, _height + addToHeight * 20); // Установка размеров для окна
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
            auto del1 = _ui->groupBox->size();
            auto del2 = _tableView->geometry().size();
            auto del3 = del1 + del2;
            int k = 0;
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
                    Requester::HandleResponse handleResponse;
                    handleResponse = std::bind(&Table::showDB, this, std::placeholders::_1);
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
