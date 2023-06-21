#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTableWidget>

class QPushButton;
class QSettings;
class QStackedWidget;

class QGridLayout;
class QLabel;

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

    public:
        Table(QSettings *iSettings, Requester* iRequester, QWidget *parent = nullptr);
        ~Table();

    Q_SIGNALS:
        void openDialog(); /// Открытие главного окна.

    private Q_SLOTS:
        void selectionChanged(const QItemSelection &, const QItemSelection &);
        void showDB(bool iResult, const QString &error);
        void showDatabase();
        void updatePersonalData(const QByteArray &iData);
        void createData(const QByteArray &iData);
        void deleteData(const QByteArray &iData);
        void updateData(const QByteArray &iData);
        void onRevertClicked();
        void onAutoUpdateClicked(bool isChecked);
        void onUpdateClicked();
        void onCancelClicked();
        void onAddUserClicked();
        void onDeleteUserClicked();
        void onCreateUserClicked();
        void onExitClicked();

    private:
        void loadSettings();
        void saveSettings();
        void resizeEvent(QResizeEvent *event) override;
        void setPersonalData(const QJsonDocument &iJson);
        static QSizePolicy GetSizePolice();

    private:
        Ui::Table *_ui = nullptr;
        QStackedWidget* _stackedWidget;
        TablePrivate *_personalData = nullptr;
        TablePrivate *_userData = nullptr;
        TableView *_tableView;
        QSettings *_settings = nullptr;
        Requester *_requester = nullptr;

        friend class TablePrivate;
    };
}

#endif // TABLE_H
