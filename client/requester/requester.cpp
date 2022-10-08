﻿#include "requester.h"

#include <QBuffer>

namespace Client
{
#define CLIENT_HTTP_TEMPLATE   "http://%1:%2//%3"
#define CLIENT_HOSTNAME        "127.0.0.1"        // Хост
#define CLIENT_PORT             5433              // Порт

    Requester::Requester(QObject *parent) :
        QObject(parent),
        _pathTemplate(CLIENT_HTTP_TEMPLATE),
        _host(CLIENT_HOSTNAME),
        _port(CLIENT_PORT),
        _sslConfig(0),
        _manager(new QNetworkAccessManager(this))
    {

    }

    Requester::~Requester()
    {
        delete _manager;
    }

    QNetworkRequest Requester::createRequest(const QString &iApi)
    {
        QNetworkRequest request;
        QString url = _pathTemplate.arg(_host).arg(_port).arg(iApi);
        request.setUrl(QUrl(url));
        request.setRawHeader("Content-Type","application/json");
        if (!_token.isEmpty())
            request.setRawHeader("Authorization", QString("%1").arg(_token).toUtf8());
        if (_sslConfig != nullptr)
            request.setSslConfiguration(*_sslConfig);
//        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");

        return request;
    }

    void Requester::sendRequest(const QString &iApi,
                                Requester::Type iType,
                                const QVariantMap &iData)
    {
        QNetworkRequest request = createRequest(iApi);
        QNetworkReply *reply;
        switch (iType)
        {
            case Type::POST:
            {
                QByteArray postDataByteArray = variantMapToJson(iData);
                reply = _manager->post(request, postDataByteArray);
                break;
            }
            case Type::GET:
            {
                reply = _manager->get(request);
                break;
            }
            case Type::DELETE:
            {
                if (iData.isEmpty())
                    reply = _manager->deleteResource(request);
                else
                    reply = sendCustomRequest(request, "DELETE", iData);
                break;
            }
            case Type::PATCH:
            {
                reply = sendCustomRequest(request, "PATCH", iData);
                break;
            }
            default:
                reply = nullptr;
                Q_ASSERT(false);
        }

        connect(reply, &QNetworkReply::finished, this, [this, reply]()
        {
            _json = parseReply(reply);

            if (onFinishRequest(reply))
            {
                emit response(true);
            }
            else
            {
                emit response(false);
            }

            reply->close();
            reply->deleteLater();
        });
    }

    QByteArray Requester::variantMapToJson(const QVariantMap& iData)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(iData);
        QByteArray data = jsonDoc.toJson();

        return data;
    }

    QNetworkReply* Requester::sendCustomRequest(QNetworkRequest &iRequest,
                                                const QString &iType,
                                                const QVariantMap &iData)
    {
        iRequest.setRawHeader("HTTP", iType.toUtf8());
        QByteArray postDataByteArray = variantMapToJson(iData);
        QBuffer *buff = new QBuffer;
        buff->setData(postDataByteArray);
        buff->open(QIODevice::ReadOnly);
        QNetworkReply* reply = _manager->sendCustomRequest(iRequest, iType.toUtf8(), buff);
        buff->setParent(reply);
        return reply;
    }

    QJsonDocument Requester::parseReply(QNetworkReply *iReply)
    {
        QJsonParseError parseError;
        QByteArray replyText = iReply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(replyText, &parseError);
        if (parseError.error != QJsonParseError::NoError)
        {
            qDebug() << replyText;
            qWarning() << "Json parse error: " << parseError.errorString();
        }

        return json;
    }

    bool Requester::onFinishRequest(QNetworkReply *iReply)
    {
        auto replyError = iReply->error();
        if (replyError == QNetworkReply::NoError )
        {
            int code = iReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if ((code >=200) && (code < 300))
            {
                return true;
            }
        }
        return false;
    }

    void Requester::handleQtNetworkErrors(QNetworkReply *iReply)
    {
        auto replyError = iReply->error();
        if ((replyError == QNetworkReply::NoError ||
              replyError == QNetworkReply::ContentNotFoundError ||
              replyError == QNetworkReply::ContentAccessDenied ||
              replyError == QNetworkReply::ProtocolInvalidOperationError) ||
              replyError == QNetworkReply::ContentNotFoundError)
        {
            qDebug() << iReply->error();
        }
    }
}
