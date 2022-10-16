#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class QJsonTableModel;

namespace Client
{
    class TableView : public QTableView
    {
        Q_OBJECT
        Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    public:
        TableView(QWidget *parent = nullptr);

        void setModel(QAbstractItemModel *model) override;

    private:
//        void resizeEvent(QResizeEvent *event) override;

    private slots:
//        void onHeaderClicked(int iColumn);
//        void itemSelectionChanged(QItemSelection, QItemSelection);
//        void itemEdited(int, int);
//        void itemDoubleClicked(int, int);

    private:
        const int _x = 0;
        const int _y = 0;
        const int _width = 800;  /// Ширина окна виджета и сцены
        const int _height = 600; /// Высота окна виджета и сцены
        QAbstractItemModel *_model = nullptr;
    };
}

#endif // TABLEVIEW_H
