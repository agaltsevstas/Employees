#ifndef REQUESTER_H
#define REQUESTER_H

#include <QJsonDocument>
#include <QMutex>
#include <QNetworkRequest>
#include <QObject>
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

        typedef std::function<void(bool, const QString&)> HandleResponse;

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
                         Type iType = Type::GET,
                         const QVariantMap &iData = QVariantMap());

        void setToken(const QString &iToken) { _token = iToken; }
        const QString getToken() const { return _token; }
        const QJsonDocument getJson() const { return _json; }
        QProgressBar *getProgressBar() const { return _progress; }

    signals:
        void response(bool iResult); // Ответ на запрос

    private slots:
        void printProgress(qint64 bytesReceived, qint64 bytesTotal);

    private:
        QString _token;
        QNetworkAccessManager *_manager;
        QJsonDocument _json;
        QProgressBar *_progress;
        QMutex mutex;
        class RequesterImpl;
        RequesterImpl *_requester;
        friend class RequesterImpl;
    };
}

#endif // REQUESTER_H
