#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Dialog;
}
QT_END_NAMESPACE

class QStatusBar;

namespace Client
{
    class Table;
    class Requester;

    class Dialog final : public QDialog
    {
        Q_OBJECT

    public:
        explicit Dialog(QWidget *parent = nullptr);
        ~Dialog();

    private:
        void loadSettings();
        void saveSettings();

    private slots:
        void showDialog();
        void on_enter_clicked();
        void on_showPassword_clicked(bool iChecked);
        void on_exit_clicked();

    private:
        void mouseMoveEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;


    private:
        int _dx, _dy;
        Ui::Dialog *_dialog = nullptr;
        Table *_table = nullptr;
        QStatusBar *_status = nullptr;
        Requester *_requester = nullptr;
    };
}
#endif // DIALOG_H
