#include "client.h"
#include "tableView.h"
#include "delegate.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLayout>
#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonTableModel>
#include <QObject>
#include <QResizeEvent>
#include <QAbstractItemModel>


namespace Client
{
    TableView::TableView( QWidget *parent) :
        QTableView(parent)
    {
        setObjectName(QString::fromUtf8("tableView"));

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setHorizontalStretch(10);
        setSizePolicy(sizePolicy);
        setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//        setGeometry(QRect(0, 0, 1000, 1000));
        setMinimumSize(QSize(1000, 657));
        setSortingEnabled(true);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setItemDelegate(new Delegate(this));
        setEditTriggers(QAbstractItemView::DoubleClicked);
//        horizontalHeader()->setHighlightSections(false);

        /* Устанавливаем названия колонок в таблице с сортировкой данных
         * */
//        for(int i = 0, j = 0; i < model->columnCount(); i++, j++)
//        {
//            _table.setHeaderData(i,Qt::Horizontal,headers[j]);
//        }

        // _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);;
        // connect(_tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotHandleDeleteButton(const QModelIndex&)));

//        connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHeaderClicked(int)));
//        connect(this, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(itemSelectionChanged(QItemSelection, QItemSelection)));
//        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(ItemSelectionChanged()));
//        connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(ItemEdited(int, int)));
//        connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(ItemDoubleClicked(int, int)));
    }

    void TableView::setEditStrategy(EditStrategy iStrategy)
    {
        _model->setEditStrategy(static_cast<QJsonTableModel::EditStrategy>(iStrategy));
    }

    void TableView::setDataModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions)
    {
        _model = new QJsonTableModel(iName, QJsonDocument(iDatabase), QJsonDocument(iPermissions), this);
        connect(_model, &QJsonTableModel::sendUpdateRequest, createData);
        connect(_model, &QJsonTableModel::sendUpdateRequest, deleteData);
        connect(_model, &QJsonTableModel::sendUpdateRequest, updateData);
        setModel(_model);
    }

    void TableView::setDataModel(const QString& iName, const QJsonDocument &iDatabase)
    {
        _model = new QJsonTableModel(iName, QJsonDocument(iDatabase), this);
        setModel(_model);
    }

    const QAbstractItemModel *TableView::getModel() const
    {
        return _model;
    }

    void TableView::setModel(QAbstractItemModel *model)
    {
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        _model = qobject_cast<QJsonTableModel*>(model);
        QTableView::setModel(model);
        resizeRowsToContents();
        resizeColumnsToContents();
        setGeometry(QRect(200, 200, 1000, 1000));
        resize(_width, _height); // Установка размеров для окна
        sortByColumn(0, Qt::SortOrder::AscendingOrder); // Может падать
//        adjustSize();
        // resize(_width, _height + addToHeight * 20); // Установка размеров для окна
    }

//    void TableView::resizeEvent(QResizeEvent *event)
//    {
////        horizontalHeader()->setCascadingSectionResizes(true);
//        QTableView::resizeEvent(event);
////        adjustSize();
//    }

//    void TableView::onHeaderClicked(int column)
//    {
//        qDebug() << "Нажатие на столбец";
////        bool sorted = qobject_cast<QJsonTableModel*>(_model)->isSortColumn(column);
////        sortByColumn(column, sorted ? Qt::SortOrder::DescendingOrder : Qt::SortOrder::AscendingOrder);
//    }

//    void TableView::itemSelectionChanged(QItemSelection, QItemSelection)
//    {
//        int k = 0;
//    }

//    void TableView::itemEdited(int, int)
//    {

//    }

//    void TableView::itemDoubleClicked(int, int)
//    {

//    }

    void TableView::submitAll()
    {
        if (_model)
            _model->submitAll();
    }

    bool TableView::addUser()
    {
        if (!_model)
            return false;


        QJsonObject record;
        record.insert(Client::Employee::id(), _model->size());

        const auto fieldNames = Client::Employee::getFieldNames();
        for (int i = 1, I = fieldNames.size(); i < I; ++i)
        {
            QPair<QString, QString> field = fieldNames[i];
            bool result = false;
            std::function<void(QWidget*)> handleLineEdit = [&](QWidget* widget)
            {
                if ( widget)
                {
                    QString value;
                    if (auto lineEdit = qobject_cast<const QLineEdit*>(widget))
                    {
                        value = lineEdit->text();
                    }
                    else if (auto comboBox = qobject_cast<const QComboBox*>(widget))
                    {
                        value = comboBox->currentText();
                    }

                    if (_model->checkField(_model->size(), i, value))
                    {
                        record.insert(field.first, value);
                        widget->setStyleSheet("");
                        result = true;
                    }
                    else
                    {
                        widget->setStyleSheet("QLineEdit { background: rgb(255,168,175); }");
                    }
                }
            };

            emit getUserData(field.first, handleLineEdit);

            if (!result)
                return false;
        }

        _model->addRow(record);
        return true;
    }

    bool TableView::deleteUser()
    {
        if (_model)
        {
            if (QItemSelectionModel *select = selectionModel())
            {
                if (select->hasSelection())
                {
                    const QModelIndexList selectedRows = select->selectedRows();
                    for (const auto &selectedRow : selectedRows)
                    {
                        if (!_model->deleteRow(selectedRow.row()))
                            return false;
                    }
                }
            }
        }

        return false;
    }
}
