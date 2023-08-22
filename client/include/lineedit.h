#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit final : public QLineEdit
{
    Q_OBJECT
public:

    explicit LineEdit(bool isView = false, QWidget *parent = nullptr);
    explicit LineEdit(const QString &text, bool isView = false, QWidget *parent = nullptr);
    ~LineEdit();

    /*!
     * \brief Запрос на автосоздание почты
     */
    void showQuestion();

Q_SIGNALS:
    void startingFocus();

private:
    void focusInEvent(QFocusEvent *event) override;

private:
    bool _isView = false;

private:
    Q_DISABLE_COPY(LineEdit)
};

#endif // LINEEDIT_H
