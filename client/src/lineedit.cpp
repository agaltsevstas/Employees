#include "lineedit.h"

#include <QFocusEvent>
#include <QMessageBox>

LineEdit::LineEdit(bool isView, QWidget *parent) :
    QLineEdit(parent),
    _isView(isView)
{

}

LineEdit::LineEdit(const QString &text, bool isView, QWidget *parent) :
    QLineEdit(text, parent),
    _isView(isView)
{

}

LineEdit::~LineEdit()
{

}

void LineEdit::showQuestion()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Автоматическое создание email"), "Хотите создать email по Фамилии, Имени и Отчеству?", QMessageBox::No | QMessageBox::Yes);
    if (reply == QMessageBox::Yes)
        emit startingFocus();
}

void LineEdit::focusInEvent(QFocusEvent *event)
{
    if (!_isView)
    {
        if (event->reason() == Qt::FocusReason::MouseFocusReason ||
            event->reason() == Qt::FocusReason::OtherFocusReason)
        {
            showQuestion();
        }
    }

    QLineEdit::focusInEvent(event);
}
