#include "validator.h"
#include "utils.h"

#include <QLineEdit>
#include <Validator>

TextValidator::TextValidator(QObject *parent) : QValidator(parent)
{

}

QValidator::State TextValidator::validate(QString &input, int&) const
{
    Utils::ToUpperandtolower(input);
    return QValidator::Acceptable;
}


UInt64Validator::UInt64Validator(quint64 minimum, quint64 maximum, Mode mode, QObject *parent) :
    QValidator(parent),
    _mode(mode),
    _min(minimum),
    _max(maximum)
{

}

void UInt64Validator::setBottom(quint64 minimum) noexcept
{
    if (_min == minimum)
        return;

    _min = minimum;
    changed();
}

void UInt64Validator::setTop(quint32 maximum) noexcept
{
    if (_max == maximum)
        return;

    _max = maximum;
    changed();
}

void UInt64Validator::setRange(quint32 minimum, quint32 maximum) noexcept
{
    setBottom(minimum);
    setTop(maximum);
}

QValidator::State UInt64Validator::validate(QString &input, int &position) const
{
    if (input.isEmpty())
    {
        _cache.clear();
        return QValidator::Acceptable;
    }

    const QString oldInput = input;
    QString newInput(input);
    newInput.resize(position);
    newInput.replace("-", "");
    bool convertOk = false;
    quint64 numInput = newInput.toULongLong(&convertOk);

    switch (_mode)
    {
        case Mode::None:
        break;
        case Mode::Date:
        {
            if (input.size() == 8 && input == newInput)
            {
                input.insert(4, "-");
                input.insert(7, "-");
            }
            else
            {
                QRegularExpression regular("(^\\d{1,4}$)|(^\\d{4}-?$)|(^(\\d{4})-(\\d{1,2})$)|(^(\\d{4})-(\\d{2})-?$)|(^(\\d{4})-(\\d{2})-(\\d{1,2})$)");
                if (!regular.match(input).hasMatch())
                    return QValidator::Invalid;

                if (_cache.size() < input.size())
                {
                    if (input.size() == 4)
                        input += "-";
                    else if (input.size() == 7)
                        input += "-";
                }
                else if (_cache.size() - 1 == input.size())
                {
                    if (_cache.back() == '-')
                        input.chop(1);
                }
            }
        }
        break;
        case Mode::Passport:
        {
            if (newInput.size() == 10 && input == newInput)
            {
                input.insert(4, "-");
            }
            else
            {
                QRegularExpression regular("(^\\d{1,4}$)|(^\\d{4}-?$)|(^(\\d{4})-(\\d{1,6})$)");
                if (!regular.match(input).hasMatch())
                    return QValidator::Invalid;

                if (_cache.size() < input.size())
                {
                    if (input.size() == 4)
                        input += "-";
                }
                else if (_cache.size() - 1 == input.size())
                {
                    if (_cache.back() == '-')
                        input.chop(1);
                }
            }
        }
        break;
        case Mode::Phone:
        {
            if (newInput.size() == 10 && input == newInput)
            {
                input.insert(3, "-");
                input.insert(7, "-");
                input.insert(10, "-");
            }
            else
            {
                QRegularExpression regular("(^\\d{1,3}$)|(^\\d{3}-?$)|"
                                           "(^(\\d{3})-(\\d{1,3})$)|(^(\\d{3})-(\\d{3})-?$)|"
                                           "(^(\\d{3})-(\\d{3})-(\\d{1,2})$)|(^(\\d{3})-(\\d{3})-(\\d{2})-?$)|"
                                           "(^(\\d{3})-(\\d{3})-(\\d{2})-(\\d{1,2})$)");
                if (!regular.match(input).hasMatch())
                    return QValidator::Invalid;

                if (_cache.size() < input.size())
                {
                    if (input.size() == 3)
                        input += "-";
                    else if (input.size() == 7)
                        input += '-';
                    else if (input.size() == 10)
                        input += '-';
                }
                else if (_cache.size() - 1 == input.size())
                {
                    if (_cache.back() == '-')
                        input.chop(1);
                }
            }
        }
        break;
    }

    _cache = input;

    if (oldInput != input)
        position = input.size();

    if (!convertOk)
        return QValidator::Invalid;
    if (numInput < _min)
        return QValidator::Intermediate;
    if (numInput > _max)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}
