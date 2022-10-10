#include "ui_dialog.h"
#include "dialog.h"
#include "table.h"
#include "requester.h"

#include <QColor>
#include <QCompleter>
#include <QDir>
#include <QMessageBox>
#include <QScreen>
#include <QStatusBar>
#include <QTimer>

#define FILENAME  "data.txt"
#define DIRECTORY "../cache/"


namespace Client
{
    Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        _dialog(new Ui::Dialog),
        _status(new QStatusBar(parent)),
        _requester(new Requester(this))
    {
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        _status->setSizePolicy(sizePolicy);

        _dialog->setupUi(this);
        _dialog->login->setPlaceholderText("Введите Логин/email");
        _dialog->password->setPlaceholderText("Введите пароль");
        _dialog->password->setEchoMode(QLineEdit::Password);
        _dialog->gridLayout->addWidget(_status, 5, 0, 1, 4);
        _dialog->rememberMe->setChecked(true);

        QPixmap loginIcon(":/images/login.png");
        QPixmap passwordIcon(":/images/password.png");
        int width = _dialog->loginIcon->width();
        int height = _dialog->loginIcon->height();
//        _dialog->loginIcon->setPixmap(loginIcon.scaled(width, height, Qt::KeepAspectRatio));
//        _dialog->passwordIcon->setPixmap(passwordIcon.scaled(width, height, Qt::KeepAspectRatio));

        readFormCache();
        QStringList list;
        for(const auto& key : _cache.keys())
            list.push_back(key);
        QCompleter* completer = new QCompleter(list);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        _dialog->login->installEventFilter(this);
        _dialog->login->setCompleter(completer);
        connect(_dialog->login, &QLineEdit::textChanged, this, &Dialog::updateLineEditStyleSheet);
        connect(_dialog->password, &QLineEdit::textChanged, this, &Dialog::updateLineEditStyleSheet);
        connect(completer, QOverload<const QString&>::of(&QCompleter::activated), [this](const QString &iText)
        {
            if (auto login = _cache.constFind(iText); login != _cache.end())
                _dialog->password->setText(login.value());
        });
        connect(_requester, SIGNAL(response(bool)), this, SLOT(authentication(bool)));
        move(qApp->primaryScreen()->availableGeometry().center()); // Установка главного окна по центру экрана
    }

    Dialog::~Dialog()
    {
        delete _dialog;
        delete _status;
        delete _requester;
        addToCache();
    }

    void Dialog::readFormCache()
    {
        QDir directory(DIRECTORY);
        if (!directory.exists())
            directory.mkpath(".");
        QFile file(QString(DIRECTORY) + FILENAME);
        file.open(QFile::ReadOnly | QFile::Text);
        if (file.isOpen())
        {
            qDebug() << "File Cache is open";
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                QStringList tokens = line.split(" ");
                if (tokens.size() == 4)
                {
                    if (tokens.at(0) == "Login:" && tokens.at(2) == "Password:")
                    {
                        QString login = tokens.at(1).mid(1, tokens.at(1).length() - 2);
                        QString password = tokens.at(3).mid(1, tokens.at(3).length() - 2);
                        _cache[login] = password;
                    }
                }
            }
        }
        else
        {
            qDebug() << "File Cache is not open";
        }
    }

    void Dialog::addToCache()
    {
        QFile file(QString(DIRECTORY) + FILENAME);
        if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        {
            qDebug() << "File Cache is not open";
            return;
        }

        QTextStream out(&file);
        for (const auto& name : _cache.keys())
        {
            out << "Login: " << "\"" << name << "\" ";
            out << "Password: " << "\"" << _cache.value(name) << "\"" << Qt::endl;
        }
        file.close();
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
        show();
    }

    void Dialog::on_enter_clicked()
    {
        const QString login = _dialog->login->text();
        const QString password = _dialog->password->text();
        if (_dialog->rememberMe->isChecked())
        {
            if (auto foundLogin = _cache.constFind(login); foundLogin != _cache.end() && foundLogin.value() != password)
            {
                QMessageBox::StandardButton reply = QMessageBox::question(this, "Пользователь с таким паролем уже существует", "Обновить пароль?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes)
                    _cache.insert(login, password);
            }
            else
                _cache.insert(login, password);
        }

        QString token = login + ":" + password;
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

