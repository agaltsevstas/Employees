#ifndef TABLE_H
#define TABLE_H

#include <QWidget>


class QStackedWidget;
class QItemSelection;

namespace Ui
{
    class Table;
}

namespace Client
{
    class TablePrivate;
    class TableView;

    class Table : public QWidget
    {
        Q_OBJECT
        using HandleResponse = std::function<void(const bool, const QVariant&)>;

    public:
        explicit Table(const QJsonDocument iData, QWidget* parent = nullptr);
        ~Table();

    Q_SIGNALS:
        /*!
         * \brief Переход к главному окну
         */
        void openDialog();

    private Q_SLOTS:
        void selectionChanged(const QItemSelection&, const QItemSelection&);
        void showDB(bool iResult, const QVariant& error);
        void showDatabase();
        void updatePersonalData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);
        void createData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);
        void deleteData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);
        void updateData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);
        void onRevertClicked();
        void onAutoUpdateClicked(bool isChecked);
        void onUpdateClicked();
        void onCancelClicked();
        void onAddUserClicked();
        void onDeleteUserClicked();
        void onRestoreUserClicked();
        void onCreateUserClicked();
        void onExitClicked();

    private:
        void loadSettings();
        void saveSettings();
        void setPersonalData(const QJsonDocument& iJson);
        void setEnabledDatabaseButtons(bool isEnable);
        bool checkChanges() const noexcept;

    private:
        Ui::Table* _ui = nullptr;
        QStackedWidget* _stackedWidget = nullptr;
        TablePrivate* _personalData = nullptr;
        TablePrivate* _userData = nullptr;
        TableView* _tableView = nullptr;

        friend class TablePrivate;
    };
}

#endif // TABLE_H
