#ifndef REQUESTER_H
#define REQUESTER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <functional>

namespace Client
{
    class Requester : public QObject
    {
        Q_OBJECT
    public:
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
                         Type iType = Type::GET,
                         const QVariantMap &iData = QVariantMap());

        void setToken(const QString &iToken) { _token = iToken; }
        const QString getToken() const { return _token; }
        const QJsonDocument getJson() const { return _json; }

    signals:
        void response(bool iResult); // Ответ на запрос

    private:
        QNetworkRequest createRequest(const QString &iApi);

        QByteArray variantMapToJson(const QVariantMap &iData);

        QNetworkReply *sendCustomRequest(QNetworkRequest &iRequest,
                                         const QString &iType,
                                         const QVariantMap &iData);

        QJsonDocument parseReply(QNetworkReply *iReply);

        bool onFinishRequest(QNetworkReply *iReply);

        void handleQtNetworkErrors(QNetworkReply *iReply);

    private:
        QString _pathTemplate;
        QString _host;
        int _port;
        QString _token;
        QSslConfiguration *_sslConfig;
        QNetworkAccessManager *_manager;
        QJsonDocument _json;
    };
}

#endif // REQUESTER_H
