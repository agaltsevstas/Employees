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
        using HandleResponse = std::function<void(const bool, const QString&)>;
        using HandleField = std::function<bool(const QString&)>;

    public:
        explicit TableView(QWidget *parent = nullptr);

        enum EditStrategy {OnFieldChange, OnManualSubmit};

        void setEditStrategy(EditStrategy iStrategy) noexcept;
        EditStrategy getEditStrategy() const noexcept;
        void setModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions);
        void setModel(const QString& iName, const QJsonDocument &iDatabase);
        [[nodiscard]] const QAbstractItemModel *getModel() const noexcept;

        void submitAll();
        bool addUser();
        bool deleteUser();
        void restoreUser();
        [[nodiscard]] std::optional<bool> canDelete();

    Q_SIGNALS:
        void sendCreateData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void sendDeleteData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void sendUpdateData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void getUserData(const QString &iFieldName, const HandleField&);

    private:
        void setModel(QAbstractItemModel *model) override;

    public Q_SLOTS:
        void valueSearchChanged(const QString &iValue);
        void clearSearchChanged();

    private:
        QList<int> _hiddenIndices;
        QJsonTableModel* _model = nullptr;
    };
}

#endif // TABLEVIEW_H
