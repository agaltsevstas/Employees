#include "quint64validator.h"

UInt64Validator::UInt64Validator(quint64 minimum, quint64 maximum, QObject *parent) :
    QValidator(parent),
    _min(minimum),
    _max(maximum)
{

}

void UInt64Validator::setBottom(quint64 minimum)
{
    if (_min == minimum)
        return;

    _min = minimum;
    changed();
}

void UInt64Validator::setTop(quint32 maximum)
{
    if (_max == maximum)
        return;

    _max = maximum;
    changed();
}

void UInt64Validator::setRange(quint32 minimum, quint32 maximum)
{
    setBottom(minimum);
    setTop(maximum);
}

QValidator::State UInt64Validator::validate(QString &input, int&) const
{
    if (input.isEmpty())
        return QValidator::Acceptable;

    bool convertOk = false;
    quint64 numInput = input.toULongLong(&convertOk);
    if (!convertOk)
        return QValidator::Invalid;
    if (numInput < _min)
        return QValidator::Intermediate;
    if (numInput > _max)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}
