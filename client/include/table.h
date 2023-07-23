#ifndef TABLE_H
#define TABLE_H

#include <QWidget>

class QPushButton;
class QStackedWidget;
class QGridLayout;
class QLabel;
class QItemSelection;
class Settings;

namespace Ui
{
    class Table;
}

namespace Client
{
    class Requester;
    class TablePrivate;
    class TableView;

    class Table : public QWidget
    {
        Q_OBJECT
        typedef std::function<void(const bool, const QString&)> HandleResponse;

    public:
        explicit Table(Requester* iRequester, QWidget *parent = nullptr);
        ~Table();

    Q_SIGNALS:
        void openDialog(); /// Открытие главного окна.

    private Q_SLOTS:
        void selectionChanged(const QItemSelection &, const QItemSelection &);
        void showDB(bool iResult, const QString &error);
        void showDatabase();
        void updatePersonalData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void createData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void deleteData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
        void updateData(const QByteArray &iData, const HandleResponse &handleResponse = Q_NULLPTR);
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
        void resizeEvent(QResizeEvent *event) override;
        void setPersonalData(const QJsonDocument &iJson);
        [[nodiscard]] static QSizePolicy GetSizePolice() noexcept;

    private:
        Ui::Table *_ui = nullptr;
        QStackedWidget* _stackedWidget = nullptr;
        TablePrivate *_personalData = nullptr;
        TablePrivate *_userData = nullptr;
        TableView *_tableView = nullptr;
        Requester *_requester = nullptr;

        friend class TablePrivate;
    };
}

#endif // TABLE_H
