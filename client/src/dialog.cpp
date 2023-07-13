#include "ui_dialog.h"
#include "cache.h"
#include "dialog.h"
#include "requester.h"
#include "session.h"
#include "table.h"

#include <QColor>
#include <QCompleter>
#include <QMessageBox>
#include <QScreen>
#include <QStatusBar>
#include <QTimer>
#include <QProgressBar>
#include <Settings>


#define DIRECTORY "../settings/"


namespace Client
{
    Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        _dialog(new Ui::Dialog),
        _status(new QStatusBar(this)),
        _requester(new Requester(this))
    {   
        _dialog->setupUi(this);

        _dialog->gridLayout->addWidget(_status, 5, 0, 1, 4);
        _dialog->gridLayout->addWidget(_requester->getProgressBar(), 6, 0, 1, 4);

        connect(_dialog->login, &QLineEdit::textChanged, this, &Dialog::updateLineEditStyleSheet);
        connect(_dialog->password, &QLineEdit::textChanged, this, &Dialog::updateLineEditStyleSheet);

        loadSettings();
    }

    Dialog::~Dialog()
    {
        saveSettings();

        delete _dialog;
    }

    void Dialog::loadSettings()
    {
        QCompleter* completer = new QCompleter(Session::getSession().Cache().getLogins(), _dialog->login);
        completer->setCaseSensitivity(Qt::CaseInsensitive);

        _dialog->login->setCompleter(completer);
        _dialog->login->installEventFilter(this);
        _dialog->login->setPlaceholderText("Введите Логин/email");
        _dialog->password->setPlaceholderText("Введите пароль");
        _dialog->password->setEchoMode(QLineEdit::Password);
        _dialog->rememberMe->setChecked(true);
        _status->setSizePolicy(_dialog->authorization->sizePolicy());
        // Установка главного окна по центру экрана по умолчанию
        move(Session::getSession().Settings().value("centerDialog", qApp->primaryScreen()->availableGeometry().center()).toPoint());

//        QPixmap loginIcon(":/images/login.png");
//        QPixmap passwordIcon(":/images/password.png");
//        int width = _dialog->loginIcon->width();
//        int height = _dialog->loginIcon->height();
//        _dialog->loginIcon->setPixmap(loginIcon.scaled(width, height, Qt::KeepAspectRatio));
//        _dialog->passwordIcon->setPixmap(passwordIcon.scaled(width, height, Qt::KeepAspectRatio));

        connect(completer, QOverload<const QString&>::of(&QCompleter::activated), [this](const QString &iLogin)
        {
                _dialog->password->setText(Session::getSession().Cache().getPassword(iLogin));
        });
    }

    void Dialog::saveSettings()
    {
        Session::getSession().Settings().setValue("centerDialog", geometry().center());
    }

    void Dialog::updateLineEditStyleSheet()
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
        if (lineEdit->text().isEmpty())
        {
            lineEdit->setStyleSheet("width: 100%; "
                                    "height: 40px; "
                                    "margin-top: 7px; "
                                    "font-size: 14px; "
                                    "color: gray; "
                                    "outline: none; "
                                    "border: 1px solid rgba(0, 0, 0, .49); "

                                    "padding-left: 20px; "
                                    "background-clip: padding-box; "
                                    "border-radius: 6px; "

                                    "background-image: -webkit-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -moz-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -o-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -ms-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); ");
        }
        else
        {
            lineEdit->setStyleSheet("width: 100%; "
                                    "height: 40px; "
                                    "margin-top: 7px; "
                                    "font-size: 14px; "
                                    "color: black; "
                                    "outline: none; "
                                    "border: 1px solid rgba(0, 0, 0, .49); "

                                    "padding-left: 20px; "
                                    "background-clip: padding-box; "
                                    "border-radius: 6px; "

                                    "background-image: -webkit-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -moz-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -o-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: -ms-linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%); "
                                    "background-image: linear-gradient(bottom, #FFFFFF 0%, #F2F2F2 100%);");
        }
    }


    void Dialog::authentication(bool iResult)
    {
        if (iResult)
        {
            qDebug() << "Вход успешно выполнен!";
            _status->setStyleSheet("color: blue");
            _status->showMessage("Вход успешно выполнен!", 1000);

            _table = new Table(_requester);
            connect(_table, &Table::openDialog, this, &Dialog::showDialog);
            QTimer::singleShot(1000, _table, SLOT(show()));
            QTimer::singleShot(1000, this, SLOT(close()));
        }
        else
        {
            qDebug() << "Введен неверный логин или пароль!";

            _status->setStyleSheet("color: red");
            _status->showMessage("Введен неверный логин или пароль!", 1000);
        }
    }

    void Dialog::showDialog()
    {
        delete _table;
        disconnect(_requester, SIGNAL(response(bool)), this, SLOT(authentication(bool)));
        _requester->sendRequest("logout");
        _dialog->gridLayout->addWidget(_requester->getProgressBar(), 6, 0, 1, 4);
        show();
    }

    void Dialog::on_enter_clicked()
    {
        const QString login = _dialog->login->text();
        const QString password = _dialog->password->text();
        if (_dialog->rememberMe->isChecked())
        {
            if (Session::getSession().Cache().findUser(login, password))
            {
                QMessageBox::StandardButton reply = QMessageBox::question(this, "Пользователь с таким паролем уже существует", "Обновить пароль?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes)
                    Session::getSession().Cache().addUser(login, password);
            }
            else
                Session::getSession().Cache().addUser(login, password);
        }

        QString token = login + ":" + password;
        connect(_requester, SIGNAL(response(bool)), this, SLOT(authentication(bool)));
        _requester->setToken(token);
        _requester->sendRequest("login");
    }

    void Dialog::on_rememberMe_clicked()
    {

    }

    void Dialog::on_showPassword_clicked(bool iChecked)
    {
        _dialog->password->setEchoMode(iChecked ? QLineEdit::Normal : QLineEdit::Password);
    }

    void Dialog::on_exit_clicked()
    {
        close();
    }
}

