#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>


class Delegate final : public QStyledItemDelegate
{

public:
    explicit Delegate(QObject* parent = nullptr) noexcept;

private:
    [[nodiscard("createEditor")]] QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // DELEGATE_H
