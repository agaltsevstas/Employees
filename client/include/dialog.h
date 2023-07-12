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

class Cache;
class QStatusBar;
class Settings;

namespace Client
{
    class Table;
    class Requester;

    class Dialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit Dialog(QWidget *parent = nullptr);
        ~Dialog();

    private:
        void loadSettings();
        void saveSettings();

    private slots:
        void updateLineEditStyleSheet();
        void authentication(bool iResult);
        void showDialog();
        void on_enter_clicked();
        void on_rememberMe_clicked();
        void on_showPassword_clicked(bool iChecked);
        void on_exit_clicked();

    private:
        Ui::Dialog *_dialog = nullptr;
        Table *_table = nullptr;
        QStatusBar *_status = nullptr;
        Settings &_settings;
        Cache &_cache;
        Requester *_requester = nullptr;
    };
}
#endif // DIALOG_H
