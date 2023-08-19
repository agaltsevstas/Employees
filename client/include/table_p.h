#ifndef TABLE_P_H
#define TABLE_P_H

#include <QWidget>

class Cache;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QJsonArray;

namespace Client
{
    class TablePrivate final : public QWidget
    {
        Q_OBJECT
        using HandleField = std::function<bool(const QString&)>;
        using HandleResponse = std::function<void(const bool, const QString&)>;

    public:
        TablePrivate(const QString &iName, const QJsonDocument &iData, const QJsonDocument &iPersonalPermissions, const QJsonDocument &iPermissions, QWidget *parent = nullptr);

        explicit TablePrivate(const QString &iName, QWidget *parent = nullptr);
        ~TablePrivate();

        enum EditStrategy {OnFieldChange, OnManualSubmit};

        void setEditStrategy(EditStrategy iStrategy);
        [[nodiscard]] inline EditStrategy getEditStrategy() const noexcept { return _strategy; }
        bool getUserData();
        void submitAll();
        bool checkChanges() const noexcept;

    Q_SIGNALS:
        void sendRequest(const QByteArray &iRequest, const HandleResponse &handleResponse = Q_NULLPTR);
        void sendValueSearch(const QString &iValue);
        void sendClearSearch();
        void logout();

    public Q_SLOTS:
        void sendUserData(const QString &iFieldName, const HandleField &handleField = Q_NULLPTR);
        void onResetDataClicked();
        void onSearchClicked();

    private Q_SLOTS:
        void createEmail();

    private Q_SLOTS:
        void update(const QString &iValue);

    private:
        bool _isRoleChanged = false;
        QString _name;
        EditStrategy _strategy = OnFieldChange;
        QList<QPair<QString, QString>> _dataCache;
        QScopedPointer<QJsonArray> _recordsCache;
    };
}

#endif // TABLE_P_H

