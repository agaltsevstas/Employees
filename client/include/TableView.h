#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
class QJsonTableModel;


namespace Client
{
    class TableView final : public QTableView
    {
        Q_OBJECT
        Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)

    public:
        explicit TableView(QWidget *parent = nullptr);

        enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

        void setEditStrategy(EditStrategy iStrategy) noexcept;
        void setModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions);
        void setModel(const QString& iName, const QJsonDocument &iDatabase);
        [[nodiscard]] const QAbstractItemModel *getModel() const noexcept;

        void submitAll();
        bool addUser();
        bool deleteUser();
        void restoreUser();
        [[nodiscard]] std::optional<bool> canDelete();

        std::function<void(const QByteArray &iData)> createData;
        std::function<void(const QByteArray &iData)> deleteData;
        std::function<void(const QByteArray &iData)> updateData;

    Q_SIGNALS:
        void getUserData(const QString &iFieldName, const std::function<void(QWidget*)>& handleLineEdit);

    private:
        void setModel(QAbstractItemModel *model) override;

    public Q_SLOTS:
        void valueSearchChanged(const QString &iValue);
        void clearSearchChanged();

    private:
        const int _x = 0;
        const int _y = 0;
        const int _width = 800;  /// Ширина окна виджета и сцены
        const int _height = 600; /// Высота окна виджета и сцены
        QList<int> _hiddenIndices;
        QJsonTableModel* _model = nullptr;
    };
}

#endif // TABLEVIEW_H
