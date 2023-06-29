#ifndef TABLE_P_H
#define TABLE_P_H

#include <QWidget>

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

    public:
        TablePrivate(const QString &iName, const QJsonDocument &iData, const QJsonDocument &iPersonalPermissions, const QJsonDocument &iPermissions, QWidget *parent = nullptr);

        explicit TablePrivate(const QString &iName, QWidget *parent = nullptr);
        ~TablePrivate();

        enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

        void setEditStrategy(EditStrategy iStrategy);
        inline EditStrategy getEditStrategy() const { return _strategy; }
        bool getUserData();
        void submitAll();

    Q_SIGNALS:
        void sendRequest(const QByteArray &iRequest);
        void sendValueSearch(const QString &iValue);
        void sendClearSearch();

    public Q_SLOTS:
        void sendUserData(const QString &iFieldName, const std::function<void(QWidget*)>& handleLineEdit);
        void onResetDataClicked();
        void onSearchClicked();

    private Q_SLOTS:
        void update();

    private:
        QString _name;
        EditStrategy _strategy = OnFieldChange;
        QList<QPair<QLabel*, QWidget*>> _data;
        QScopedPointer<QJsonArray> _recordsCache;
    };
}

#endif // TABLE_P_H

