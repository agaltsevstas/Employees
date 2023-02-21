#include "delegate.h"
#include "qjsontablemodel.h"

#include <QPainter>
#include <QAbstractItemView>
#include <QEvent>
#include <QMouseEvent>
#include <QSpinBox>
#include <QComboBox>


Delegate::Delegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    if (QAbstractItemView* tableView = qobject_cast<QAbstractItemView*>(this->parent()))
//    {
//        QModelIndex hover = tableView->indexAt(tableView->viewport()->mapFromGlobal(QCursor::pos()));
//        if (hover.row() == index.row())
//        {
//            painter->fillRect(option.rect, Qt::red);
//        }
//    }

    if (const QAbstractItemModel *model = index.model())
    {
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ? (option.state & QStyle::State_Active) ? QPalette::Normal :
                                                                                                                   QPalette::Inactive :
                                                                                                                   QPalette::Disabled;

        if (option.state & QStyle::State_Selected)
            painter->fillRect( option.rect, option.palette.color(cg, QPalette::Highlight));

        int width = star.width();
        int height = star.height();
        int x = option.rect.x();
        int y = option.rect.y() + (option.rect.height() / 2) - (height / 2);
        int rating = model->data(index, Qt::DisplayRole).toInt();
        for (int i = 0; i < rating; ++i)
        {
            painter->drawPixmap(x, y, star);
            x += width;
        }

        QPen pen = painter->pen();
        painter->setPen(option.palette.color(QPalette::Mid));
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
        painter->setPen(pen);
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
//    if (index.column() == 5)
//        return QSize(5 * star.width(), star.height()) + QSize(1, 1);
    // Since we draw the grid ourselves:
    return QStyledItemDelegate::sizeHint(option, index) + QSize(1, 1);
}

bool Delegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index)
{
//    if (index.column() != 5)
//        return QStyledItemDelegate::editorEvent(event, model, option, index);

//    if (event->type() == QEvent::MouseButtonPress)
//    {
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        int stars = qBound(0, int(0.7 + qreal(mouseEvent->position().toPoint().x()
//            - option.rect.x()) / star.width()), 5);
//        model->setData(index, QVariant(stars));
//        // So that the selection can change:
//        return false;
//    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QWidget *Delegate::createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    switch (index.column())
    {
        case 1:
        case 5:
        {
            if (const QJsonTableModel *model = qobject_cast<const QJsonTableModel*>(index.model()))
            {
                QComboBox *combobox = new QComboBox(parent);
                combobox->setDuplicatesEnabled(false);
                combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                combobox->installEventFilter(const_cast<Delegate*>(this));

                QSet<QString> uniqueSet;
                for (int i = 0, I = model->rowCount({}); i < I; ++i)
                {
                    QString value = model->data(model->index(i, index.column()), Qt::DisplayRole).toString();
                    if (!uniqueSet.contains(value))
                    {
                        uniqueSet.insert(value);
                        combobox->addItem(value);
                    }
                 }

                return combobox;
            }

            break;
        }
        case 12:
        {
            if (const QJsonTableModel *model = qobject_cast<const QJsonTableModel*>(index.model()))
            {
                QSpinBox *spinbox = new QSpinBox(parent);
                spinbox->setFrame(false);
                auto del = model->data(index, Qt::DisplayRole).toString();
                auto row = index.row();
                auto column = index.column();
                spinbox->setValue(model->data(index, Qt::DisplayRole).toInt());
                spinbox->setRange(0, 1000000);

                return spinbox;
            }

            break;
        }
        default:
            break;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void Delegate::setEditorData(QWidget *editor,
                            const QModelIndex &index) const
{
    int role = index.data(Qt::EditRole).toInt();
    switch (index.column())
    {
        case 1:
        case 5:
        {
            QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);
            auto index = comboBox->currentIndex();
            break;
        }
        case 12:
        {
            QSpinBox *spinbox = dynamic_cast<QSpinBox*>(editor);
            auto value = spinbox->value();
            break;
        }
        default:
            break;
    }

    return QStyledItemDelegate::setEditorData(editor, index);
}
