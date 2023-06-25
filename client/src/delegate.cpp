#include "delegate.h"
#include "client.h"

#include <QAbstractItemView>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QJsonTableModel>
#include <QPainter>
#include <QUInt64Validator>


QSizePolicy GetSizePolice()
{
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    return sizePolicy;
}

Delegate::Delegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

QWidget *Delegate::createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    const QJsonTableModel *model = qobject_cast<const QJsonTableModel*>(index.model());
    if (model)
    {
        switch (index.column())
        {
            case 1:
            case 5:
            {
                QComboBox *comboBox = new QComboBox(parent);
                connect(comboBox, &QComboBox::currentTextChanged, this, [&](const QString &text)->void
                {
                    qInfo() << "Текст: " << text;
    //                if (text != currentText)
    //                    const_cast<QJsonTableModel*>(model)->setData(index, text, Qt::DisplayRole);
                });

                auto *childModel = model->relationModel(index.column());
                if (childModel)
                {
                    comboBox->setModel(childModel);
                    comboBox->setToolTip(Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first]);
//                    comboBox->setModelColumn(index.column());
                    comboBox->setDuplicatesEnabled(false);
                    comboBox->installEventFilter(const_cast<Delegate*>(this));
                    comboBox->setSizePolicy(GetSizePolice());
                    comboBox->setStyleSheet(QString::fromUtf8("QComboBox {\n    border: 1px solid gray;\n}\n\nQComboBox::drop-down {\n    border-color: transparent;\n}"));
//                    QLineEdit *lineEdit = new QLineEdit(comboBox);
//                    QFont font = lineEdit->font();
//                    font.setBold(true);
//                    font.setKerning(true);
//                    lineEdit->setFont(font);
//                    comboBox->setLineEdit(lineEdit);

                    return comboBox;
                }
            }
            case 6:
            case 10:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first]);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, parent));
                return lineEdit;
            }
            case 7:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first]);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, parent));
                return lineEdit;
            }
            case 8:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setToolTip(Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first]);
                lineEdit->setText(model->data(index, Qt::DisplayRole).toString());
                lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, parent));
                return lineEdit;
            }
            case 12:
            {
                QDoubleSpinBox *spinbox = new QDoubleSpinBox(parent);
                spinbox->setToolTip(Client::Employee::helpFields()[Client::Employee::getFieldNames()[index.column()].first]);
                spinbox->setValue(model->data(index, Qt::DisplayRole).toInt());
                spinbox->setFrame(false);
                spinbox->setRange(0, 1000000);
                return spinbox;
            }
            default:
                break;
        }
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void Delegate::setEditorData(QWidget *editor,
                             const QModelIndex &index) const
{
    const QJsonTableModel *model = qobject_cast<const QJsonTableModel*>(index.model());
    if (!model)
        return;

    const QString currentText = model->data(index, Qt::DisplayRole).toString();
    switch (index.column())
    {
        case 1:
        case 5:
        {
            QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);

//            comboBox->lineEdit()->setReadOnly(true);
            break;
        }
        case 12:
        {
            QDoubleSpinBox *spinbox = dynamic_cast<QDoubleSpinBox*>(editor);
            spinbox->setBackgroundRole(QPalette::ColorRole::BrightText);
//            spinbox->setPalette();
            auto value = spinbox->value();
            break;
        }
        default:
            break;
    }

    return QStyledItemDelegate::setEditorData(editor, index);
}

//bool Delegate::editorEvent(QEvent *event, QAbstractItemModel *model,
//                           const QStyleOptionViewItem &option,
//                           const QModelIndex &index)
//{
////    if (index.column() != 5)
////        return QStyledItemDelegate::editorEvent(event, model, option, index);

////    if (event->type() == QEvent::MouseButtonPress)
////    {
////        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
////        int stars = qBound(0, int(0.7 + qreal(mouseEvent->position().toPoint().x()
////            - option.rect.x()) / star.width()), 5);
////        model->setData(index, QVariant(stars));
////        // So that the selection can change:
////        return false;
////    }

//    return QStyledItemDelegate::editorEvent(event, model, option, index);
//}

//void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
////    if (QAbstractItemView* tableView = qobject_cast<QAbstractItemView*>(this->parent()))
////    {
////        QModelIndex hover = tableView->indexAt(tableView->viewport()->mapFromGlobal(QCursor::pos()));
////        if (hover.row() == index.row())
////        {
////            painter->fillRect(option.rect, Qt::red);
////        }
////    }

//    if (const QAbstractItemModel *model = index.model())
//    {
//        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ? (option.state & QStyle::State_Active) ? QPalette::Normal :
//                                                                                                                   QPalette::Inactive :
//                                                                                                                   QPalette::Disabled;

//        if (option.state & QStyle::State_Selected)
//            painter->fillRect( option.rect, option.palette.color(cg, QPalette::Highlight));

//        int width = star.width();
//        int height = star.height();
//        int x = option.rect.x();
//        int y = option.rect.y() + (option.rect.height() / 2) - (height / 2);
//        int rating = model->data(index, Qt::DisplayRole).toInt();
//        for (int i = 0; i < rating; ++i)
//        {
//            painter->drawPixmap(x, y, star);
//            x += width;
//        }

//        QPen pen = painter->pen();
//        painter->setPen(option.palette.color(QPalette::Mid));
//        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
//        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
//        painter->setPen(pen);
//    }

//    QStyledItemDelegate::paint(painter, option, index);
//}

//QSize Delegate::sizeHint(const QStyleOptionViewItem &option,
//                         const QModelIndex &index) const
//{
////    if (index.column() == 5)
////        return QSize(5 * star.width(), star.height()) + QSize(1, 1);
//    // Since we draw the grid ourselves:
//    return QStyledItemDelegate::sizeHint(option, index) + QSize(1, 1);
//}

//void Delegate::updateEditorGeometry(QWidget *editor,
//                                    const QStyleOptionViewItem &option,
//                                    const QModelIndex &index) const
//{
//    editor->setGeometry(option.rect);
//}
