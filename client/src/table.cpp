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

namespace Client
{
    Table::Table(Requester* iRequester, QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::Table),
        _requester(iRequester)
    {
        _ui->setupUi(this);
        setPersonalData(_requester->getJson());
        _table = new QJsonTableModel(_requester->getJson());
//        _ui->tableView->setModel(_table);
//        _ui->tableView->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
//        _ui->tableView->resizeColumnsToContents();
//        _ui->tableView->verticalHeader()->setDefaultSectionSize(20);

    //    _ui->tableView->setSizeIncrement();
    //        /* Устанавливаем названия колонок в таблице с сортировкой данных
    //         * */
    //        for(int i = 0, j = 0; i < model->columnCount(); i++, j++){
    //            _table.setHeaderData(i,Qt::Horizontal,headers[j]);
    //        }
    //        // Устанавливаем сортировку по возрастанию данных по нулевой колонке
    //        model->setSort(0,Qt::AscendingOrder);

        connect(_requester, SIGNAL(response(bool)), this, SLOT(requestResponse(bool)));
    }

    void Table::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);
    }

    void Table::setPersonalData(const QJsonDocument &json)
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(_ui->PersonalData->hasHeightForWidth());

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
                    sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
                    lineEdit->setSizePolicy(sizePolicy);
                    _ui->gridLayout_2->addWidget(lineEdit, i + 2, 1, 1, 1);
                }
            }
        }
    }

    Table::~Table()
    {
        delete _ui;
        delete _table;
    }

    void Table::on_exit_clicked()
    {
        close();           // Закрытие окна
        emit openDialog(); // Вызов главного окна
    }

    void Table::requestResponse(bool iResult)
    {
        if (iResult)
        {
            qDebug() << "Ответ на запрос получен!";

        }
        else
        {
            qDebug() << "Ответ на запрос не получен!";
        }
    }
}

