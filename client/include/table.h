#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTableView>

class QAbstractTableModel;

namespace Ui
{
    class Table;
}

namespace Client
{
    class Requester;

    class Table : public QWidget
    {
        Q_OBJECT
    public:
        Table(Requester* iRequester, QWidget *parent = nullptr);
        ~Table();

    signals:
        void openDialog(); /// Открытие главного окна.

    private slots:
        void showDB(bool iResult);
        void showDatabase();
        void on_exit_clicked();
        void on_update_clicked();

    private:
        void resizeEvent(QResizeEvent *event) override;
        void setPersonalData(const QJsonDocument &json);
        static QSizePolicy GetSizePolice();

    private:
        Ui::Table *_ui = nullptr;
        QAbstractTableModel* _personalDataModel = nullptr;
        QAbstractTableModel* _databaseModel = nullptr;
        QTableView *_tableView;
        Requester* _requester = nullptr;
    };
}

#endif // TABLE_H
