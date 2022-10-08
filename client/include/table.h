#ifndef TABLE_H
#define TABLE_H

#include <QWidget>

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
        void requestResponse(bool iResult);
        void on_exit_clicked();

    private:
        void resizeEvent(QResizeEvent *event) override;
        void setPersonalData(const QJsonDocument &json);

    private:
        Ui::Table *_ui = nullptr;
        QAbstractTableModel* _table = nullptr;
        Requester* _requester = nullptr;
    };
}

#endif // TABLE_H
