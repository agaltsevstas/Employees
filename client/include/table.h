#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTableView>
#include <QTableWidget>

class PersonalDataTable;
class QJsonTableModel;
class QPushButton;
class QSettings;

namespace Ui
{
    class Table;
}

namespace Client
{
    class Requester;
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
        void showDB(bool iResult, const QString &error);
        void showDatabase();
        void updatePersonalData(const QByteArray &iData);
        void updateData(const QByteArray &iData);
        void on_revert_clicked();
        void on_autoUpdate_clicked(bool isChecked);
        void on_update_clicked();
        void onCreateUserClicked();
        void onDeleteUserClicked();
        void on_exit_clicked();

    private:
        void loadSettings();
        void saveSettings();
        void resizeEvent(QResizeEvent *event) override;
        void setPersonalData(const QJsonDocument &iJson);
        static QSizePolicy GetSizePolice();

    private:
        Ui::Table *_ui = nullptr;
        PersonalDataTable *_personalData = nullptr;
        QJsonTableModel *_databaseModel = nullptr;
        TableView *_tableView;
        QSettings *_settings = nullptr;
        Requester *_requester = nullptr;
    };
}

#endif // TABLE_H
