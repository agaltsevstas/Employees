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

        enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

        void setEditStrategy(EditStrategy iStrategy);
        void setDataModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions);
        void setDataModel(const QString& iName, const QJsonDocument &iDatabase);
        const QAbstractItemModel *getModel() const;

        void submitAll();
        bool addUser();
        bool deleteUser();
        void restoreUser();
        std::optional<bool> canDelete();

        std::function<void(const QByteArray &iData)> createData;
        std::function<void(const QByteArray &iData)> deleteData;
        std::function<void(const QByteArray &iData)> updateData;

    Q_SIGNALS:
        void getUserData(const QString &iFieldName, const std::function<void(QWidget*)>& handleLineEdit);

    private:
        void setModel(QAbstractItemModel *model) override;
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
        QJsonTableModel* _model;
    };
}

#endif // TABLEVIEW_H
