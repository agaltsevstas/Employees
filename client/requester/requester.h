#ifndef REQUESTER_H
#define REQUESTER_H

#include <QJsonDocument>
#include <QMutex>
#include <QNetworkRequest>
#include <QObject>
#include <QScopedPointer>
#include <QVariantMap>


class QProgressBar;
class QNetworkAccessManager;
class QSslConfiguration;
class QNetworkReply;

namespace Client
{
    class Requester : public QObject
    {
        Q_OBJECT
    public:

        typedef std::function<void(const bool, const QString&)> HandleResponse;

        enum class Type
        {
            POST,
            GET,
            PATCH,
            DELETE
        };

        explicit Requester(QObject *parent = 0);
        ~Requester();

        void sendRequest(const QString &iApi,
                         const HandleResponse &handleResponse = Q_NULLPTR,
                         const Type iType = Type::GET,
                         const QByteArray &iData = {});

        void setToken(const QString &iToken) { _token = iToken; }
        const QString getToken() const { return _token; }
        const QJsonDocument getJson() const { return _json; }
        QProgressBar *getProgressBar() const { return _progress.get(); }

    signals:
        void response(bool iResult); // Ответ на запрос

    private slots:
        void printProgress(qint64 bytesReceived, qint64 bytesTotal);

    private:
        QString _token;
        QNetworkAccessManager *_manager;
        QJsonDocument _json;
        QScopedPointer<QProgressBar> _progress;
        QMutex mutex;
        class RequesterImpl;
        QScopedPointer<RequesterImpl> _requester;
        friend class RequesterImpl;
    };
}

#endif // REQUESTER_H
