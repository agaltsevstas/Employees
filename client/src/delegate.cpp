#include "delegate.h"
#include "client.h"

#include <LineEdit>
#include <QApplication>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMouseEvent>
#include <QJsonTableModel>
#include <QPainter>
#include <Validator>


static QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

static constexpr QSizePolicy GetSizePolice() noexcept
{
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    return sizePolicy;
}

Delegate::Delegate(QObject* parent) noexcept : QStyledItemDelegate(parent)
{

}

QWidget *Delegate::createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (const auto& model = qobject_cast<const QJsonTableModel*>(index.model()))
    {
        const QString toolTip = Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first];
        QString placeholderText = toolTip;
        switch (index.column())
        {
            case 11:
                placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                [[fallthrough]];
            case 2:
            case 3:
            case 4:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new TextValidator(parent));
                return lineEdit;
            }
            case 1:
            case 5:
            {
                if (auto *childModel = model->relationModel(index.column()))
                {
                    QComboBox *comboBox = new QComboBox(parent);
                    comboBox->setModel(childModel);
                    comboBox->setToolTip(toolTip);
                    comboBox->setDuplicatesEnabled(false);
                    comboBox->installEventFilter(const_cast<Delegate*>(this));
                    comboBox->setSizePolicy(GetSizePolice());
                    comboBox->setStyleSheet(QString::fromUtf8("QComboBox {border: 1px solid gray; padding: 0px;} QComboBox::drop-down {border-color: transparent;}"));
//                    comboBox->setEditable(true); // Можно вводить текст с помощью подсказок

                    return comboBox;
                }
                break;
            }
            case 6:
            case 10:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, parent));
                return lineEdit;
            }
            case 7:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, parent));
                return lineEdit;
            }
            case 8:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, parent));
                return lineEdit;
            }
            case 9:
            {
                LineEdit *lineEdit = new LineEdit(true, parent);
                connect(lineEdit, &LineEdit::startingFocus, [&model, &index]()
                {
                    const_cast<QJsonTableModel*>(model)->createEmail(index.row());
                });
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->showQuestion();
                return lineEdit;
            }
            case 12:
            {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent);
                spinBox->setToolTip(toolTip);
                spinBox->setValue(model->data(index, Qt::DisplayRole).toInt());
                spinBox->setFrame(false);
                spinBox->setRange(0, 1000000);
                return spinBox;
            }
            case 13:
            {
                placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                return lineEdit;
            }
            break;
            default:
                break;
        }
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_MouseOver)
    {
        painter->fillRect(option.rect, QColor(165, 205, 255));
    }

    QPen pen = painter->pen();
    painter->setPen(option.palette.color(QPalette::Mid));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    painter->setPen(std::move(pen));

    QStyledItemDelegate::paint(painter, option, index);
}

/* Фильтр помогает открывать QComboBox по двойному клику вместо тройного */
bool Delegate::eventFilter(QObject *object, QEvent *event)
{
    if (qobject_cast<const QComboBox*>(object) && event && event->type() == QEvent::FocusIn)
    {
        QMouseEvent *currentEvent = static_cast<QMouseEvent*>(event);
        QMouseEvent nextEvent(QEvent::MouseButtonPress, currentEvent->pos(), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(object, &nextEvent);
    }

    return QStyledItemDelegate::eventFilter(object, event);
}
