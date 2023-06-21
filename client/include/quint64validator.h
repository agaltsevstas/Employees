#ifndef QUINT64VALIDATOR_H
#define QUINT64VALIDATOR_H

#include <QValidator>

class UInt64Validator : public QValidator
{
    Q_OBJECT
public:
    UInt64Validator(quint64 min, quint64 max, QObject *parent = nullptr);
    inline quint64 bottom() const { return _min; }
    inline quint64 top() const { return _max; }
    void setBottom(quint64 min);
    void setTop(quint32 max);

    void setRange(quint32 min, quint32 max);

    QValidator::State validate(QString &input, int&) const override;
private:
    quint64 _min;
    quint64 _max;
};

#endif // QUINT64VALIDATOR_H
