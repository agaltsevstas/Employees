#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>


class Delegate : public QStyledItemDelegate
{

public:
    explicit Delegate(QObject* parent = nullptr) noexcept;

private:

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

//    bool editorEvent(QEvent *event, QAbstractItemModel *model,
//                     const QStyleOptionViewItem &option,
//                     const QModelIndex &index) override;


//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const override;

//    void updateEditorGeometry(QWidget *editor,
//                              const QStyleOptionViewItem &option,
//                              const QModelIndex &index) const;

private:
    QPixmap star;
};

#endif // DELEGATE_H
