#ifndef COOKIE_H
#define COOKIE_H

#include <QMap>


class Cookie final
{
    Q_DISABLE_COPY(Cookie);
    friend class Session;

public:
    bool isValid() const;
    QString getValidToken();
    void add(const QString &iKey, const QString &iValue) noexcept;
    QString getUserName() const;
    const QMap<QString, QString>& get() const noexcept { return _data; }
    void clear() noexcept;
    bool isEmpty() const noexcept;

private:
    static Cookie &Instance() noexcept;
    Cookie();
    ~Cookie();

    QMap<QString, QString> _data;
};

#endif // COOKIE_H
