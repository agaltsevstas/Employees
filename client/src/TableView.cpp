#include "tableView.h"
#include "delegate.h"

#include <QObject>
#include <QHeaderView>
#include <QResizeEvent>
#include <QJsonTableModel>


namespace Client
{
    TableView::TableView(QWidget *parent) : QTableView(parent)
    {
        setObjectName(QString::fromUtf8("tableView"));

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setHorizontalStretch(10);
        setSizePolicy(sizePolicy);
        setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//        setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
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

    void TableView::setModel(QAbstractItemModel *model)
    {
//        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        QTableView::setModel(_model = model);
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
}
