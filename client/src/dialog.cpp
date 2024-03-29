#include "ui_dialog.h"
#include "cache.h"
#include "cookie.h"
#include "dialog.h"
#include "session.h"
#include "table.h"

#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProgressBar>
#include <QStatusBar>
#include <QTimer>
#include <Requester>
#include <Settings>


extern QScopedPointer<Client::Requester> requester;

namespace Client
{
    Dialog::Dialog(QWidget* parent) :
        QDialog(parent),
        _dialog(new Ui::Dialog)
    {   
        _dialog->setupUi(this);
        setWindowFlag(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        auto outerFrameEffect = new QGraphicsDropShadowEffect();
        outerFrameEffect->setBlurRadius(25);
        outerFrameEffect->setXOffset(0);
        outerFrameEffect->setYOffset(0);
        outerFrameEffect->setColor(QColor(234, 221, 186, 100));
        _dialog->outerFrame->setGraphicsEffect(std::move(outerFrameEffect));

        auto innerFrameEffect = new QGraphicsDropShadowEffect();
        innerFrameEffect->setBlurRadius(25);
        innerFrameEffect->setXOffset(0);
        innerFrameEffect->setYOffset(0);
        innerFrameEffect->setColor(QColor(105, 118, 132, 100));
        _dialog->innerFrame->setGraphicsEffect(std::move(innerFrameEffect));

        auto enterEffect = new QGraphicsDropShadowEffect();
        enterEffect->setBlurRadius(25);
        enterEffect->setXOffset(3);
        enterEffect->setYOffset(3);
        enterEffect->setColor(QColor(105, 118, 132, 100));
        _dialog->enter->setGraphicsEffect(std::move(enterEffect));

        auto exitEffect = new QGraphicsDropShadowEffect();
        exitEffect->setBlurRadius(25);
        exitEffect->setXOffset(3);
        exitEffect->setYOffset(3);
        exitEffect->setColor(QColor(235, 255, 255, 90));
        _dialog->exit->setGraphicsEffect(std::move(exitEffect));

        _status = new QStatusBar(this);
        _status->setObjectName("status");
        _status->setGeometry(QRect(290, 395, 270, 20));
        _status->setStyleSheet("background:rgba(0, 0, 0, 0);");

        loadSettings();

        if (Session::getSession().Cookie().isValid())
        {
            const QString userName = Session::getSession().Cookie().getUserName();
            if (!userName.isEmpty())
            {
                for (const auto& login : Session::getSession().Cache().getLogins())
                {
                    if (userName == login)
                    {
                        _dialog->login->setText(login);
                        _dialog->password->setText(Session::getSession().Cache().getPassword(login));
                    }
                }
            }

            on_enter_clicked();
        }
    }

    Dialog::~Dialog()
    {
        saveSettings();

        delete _dialog;
    }

    void Dialog::loadSettings()
    {
        qInfo() << "Загрузка настроек для диалога";
        QCompleter* completer = new QCompleter(Session::getSession().Cache().getLogins(), _dialog->login);
        completer->setCaseSensitivity(Qt::CaseInsensitive);

        _dialog->login->setCompleter(completer);
        _dialog->login->installEventFilter(this);
        _dialog->password->setEchoMode(QLineEdit::Password);
        _dialog->rememberMe->setChecked(true);
        // Установка главного окна по центру экрана по умолчанию
        move(Session::getSession().Settings().value("centerDialog", qApp->primaryScreen()->availableGeometry().center()).toPoint());

        connect(completer, QOverload<const QString&>::of(&QCompleter::activated), [this](const QString& iLogin)
        {
            _dialog->password->setText(Session::getSession().Cache().getPassword(iLogin));
        });
    }

    void Dialog::saveSettings()
    {
        qInfo() << "Сохранение настроек для диалога";
        Session::getSession().Settings().setValue("centerDialog", geometry().center() - QPoint(width() / 2, height() / 2));
    }

    void Dialog::showDialog()
    {
        qInfo() << "Вход в диалог";
        delete _table;
        requester->getResource("logout");
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
        requester->setToken(std::move(token));

        requester->getResource("login", [this](const bool iResult, const QVariant& iData)
        {
            if (iResult)
            {
                _status->setStyleSheet("background:rgba(0, 0, 0, 0); color:rgba(255, 255, 255, 210);");
                _status->showMessage("Вход успешно выполнен!", 1000);

                _table = new Table(iData.toJsonDocument());
                connect(_table, &Table::openDialog, this, &Dialog::showDialog);
                QTimer::singleShot(1000, _table, SLOT(show()));
                QTimer::singleShot(1000, this, SLOT(close()));
            }
            else
            {
                if (iData.toString() == "Connection refused")
                {
                    qWarning() << "Ошибка: " << iData.toString();
                    QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), iData.toString(), QMessageBox::NoButton, this);
                    QTimer::singleShot(1000, &warning, &QMessageBox::close);
                    warning.exec();
                }
                else
                {
                    qWarning() << "Введен неверный логин или пароль";
                    _status->setStyleSheet("background:rgba(0, 0, 0, 0); color: red;");
                    _status->showMessage("Введен неверный логин или пароль!", 1000);
                }
            }
        });
    }

    void Dialog::on_showPassword_clicked(bool iChecked)
    {
        qInfo() << "Показать пароль: " << iChecked;
        _dialog->password->setEchoMode(iChecked ? QLineEdit::Normal : QLineEdit::Password);
    }

    void Dialog::on_exit_clicked()
    {
        qInfo() << "Выход из диалога";
        close();
    }

    void Dialog::mouseMoveEvent(QMouseEvent* event)
    {
        if (event->buttons() | Qt::LeftButton )
        {
            setGeometry(pos().x() + (event->position().x() - _dx),
                        pos().y() + (event->position().y() - _dy),
                        width(),
                        height());
        }
    }

    void Dialog::mousePressEvent(QMouseEvent* event)
    {
        if (event->button() == Qt::LeftButton )
        {
            _dx = event->position().x();
            _dy = event->position().y();
            setCursor(Qt::OpenHandCursor);
        }
    }

    void Dialog::mouseReleaseEvent(QMouseEvent* event)
    {
        if (event->button() == Qt::LeftButton )
            setCursor(Qt::ArrowCursor);
    }
}

