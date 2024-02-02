#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>


class Error {};

class Exception
{
public:
    explicit Exception(const QString &iText) : _text(iText)  {}

    virtual const QString& what() const noexcept { return _text; }
private:
    QString _text;
};

class CreateDBError: public Exception
{
public:
    explicit CreateDBError(const QString& iText) noexcept : Exception(iText) {}

};

class CreateTableError: public Exception
{
public:
    explicit CreateTableError(const QString& iText) noexcept : Exception(iText) {}
};

class OpenDBError: public Exception
{
public:
    explicit OpenDBError(const QString& iText) noexcept : Exception(iText) {}
};

class OpenTableError: public Exception
{
public:
    explicit OpenTableError(const QString& iText) noexcept : Exception(iText) {}
};

#endif // EXCEPTION_H
