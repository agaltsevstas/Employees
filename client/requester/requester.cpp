#include "requester.h"

#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>


namespace Client
{
    #define CLIENT_HTTP_TEMPLATE   "http://%1:%2//%3"
    #define CLIENT_HOSTNAME        "127.0.0.1"        // Хост
    #define CLIENT_PORT             5433              // Порт

    class NetworkCookieJar: public QNetworkCookieJar
    {
    public:
         explicit NetworkCookieJar(QObject *parent = nullptr) :
            QNetworkCookieJar(parent)
         {

         }

         virtual ~NetworkCookieJar()
         {

         }

         QList<QNetworkCookie> getAllCookies() const
         {
             return allCookies();
         }

         void setAllCookies(const QList<QNetworkCookie>& cookies)
         {
             QNetworkCookieJar::setAllCookies(cookies);
         }

         bool isEmpty()
         {
             return allCookies().isEmpty();
         }

         void clear()
         {
             setAllCookies({});
         }
    };

    class Requester::RequesterImpl
    {
    public:
        RequesterImpl(Requester *iRequester) :
          _pathTemplate(CLIENT_HTTP_TEMPLATE),
          _host(CLIENT_HOSTNAME),
          _port(CLIENT_PORT),
          _sslConfig(0),
          _requester(*iRequester)
        {}

        QNetworkRequest createRequest(const QString &iApi);

        [[nodiscard]] QJsonDocument parseReply(QNetworkReply *iReply);

        [[nodiscard]] QNetworkReply *sendCustomRequest(QNetworkRequest &iRequest,
                                                       const QString &iType,
                                                       const QByteArray &iData);

        bool checkFinishRequest(QNetworkReply *iReply);

        NetworkCookieJar* getCookie() { return &_cookie; };

    private:
        QString _pathTemplate;
        QString _host;
        int _port;
        QSslConfiguration *_sslConfig;
        NetworkCookieJar _cookie;
        Requester &_requester;
    };

    QJsonDocument Requester::RequesterImpl::parseReply(QNetworkReply *iReply)
    {
        QByteArray replyData = iReply->readAll();
        if (replyData.isEmpty())
        {
            qInfo() << "Пустые данные";
            return {};
        }

        // TODO: Сделать вывод в отдельном потоке, иначе зависает из-за большого вывода информации
//        qInfo() << "Получены данные: " + QString::fromUtf8(replyData);
        QJsonDocument jsonDocument = QJsonDocument::fromJson(replyData);
        if (jsonDocument.isEmpty())
        {
            QJsonArray records;

            while (!replyData.isEmpty())
            {
                QJsonParseError parseError;
                auto index = replyData.indexOf("\n}\n") + 3;
                QJsonDocument json = QJsonDocument::fromJson(replyData.left(index), &parseError);
                if (parseError.error != QJsonParseError::NoError)
                {
                    qWarning() << "Json parse error: " << parseError.errorString();
                    continue;
                }
                if (json.isObject())
                {
                    records.push_back(json.object());
                }

                replyData = replyData.mid(index);
            }

            return QJsonDocument(std::move(records));
        }

        return jsonDocument;
    }

    QNetworkRequest Requester::RequesterImpl::createRequest(const QString &iApi)
    {
        QNetworkRequest request;
        QString url = _pathTemplate.arg(_host).arg(_port).arg(iApi);
        request.setUrl(QUrl(url));
        request.setRawHeader("Content-Type","application/json");
        if (!_cookie.isEmpty())
            request.setRawHeader("Authorization", QString("Bearer %1").arg(_requester._token.toUtf8().toBase64()).toLocal8Bit());
//          request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(_requester._requester->_cookie));
        else if (!_requester._token.isEmpty())
            request.setRawHeader("Authorization", QString("Basic %1").arg(_requester._token.toUtf8().toBase64()).toLocal8Bit());
        if (_sslConfig != Q_NULLPTR)
            request.setSslConfiguration(*_sslConfig);
        return request;
    }

    QNetworkReply* Requester::RequesterImpl::sendCustomRequest(QNetworkRequest &iRequest,
                                                const QString &iType,
                                                const QByteArray &iData)
    {
        iRequest.setRawHeader("HTTP", iType.toUtf8());
        QBuffer *buff = new QBuffer;
        buff->setData(iData);
        buff->open(QIODevice::ReadOnly);
        QNetworkReply* reply = _requester._manager->sendCustomRequest(iRequest, iType.toUtf8(), buff);
        buff->setParent(reply);
        return reply;
    }

    bool Requester::RequesterImpl::checkFinishRequest(QNetworkReply *iReply)
    {
        auto replyError = iReply->error();
        if (replyError == QNetworkReply::NoError)
        {
            int code = iReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if ((code >= 200) && (code < 300))
            {
                return true;
            }
        }

        return false;
    }


    Requester::Requester(QObject *parent) :
        QObject(parent),
        _manager(new QNetworkAccessManager(this)),
        _progress(new QProgressBar()),
        _requester(new RequesterImpl(this))
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setHorizontalStretch(10);
        _progress->setRange(0, 100);
        _progress->setSizePolicy(sizePolicy);
    }

    Requester::~Requester()
    {

    }

    void Requester::printProgress(qint64 bytesReceived, qint64 bytesTotal)
    {
//        QMutexLocker locker(&mutex);
        if (bytesTotal > 0 && bytesReceived > 0)
        {
            qDebug() << "Получено байт" << bytesReceived << " из " << bytesTotal;
            _progress->setValue((int)(bytesReceived * 100 / bytesTotal));
        }
    }

    void Requester::sendRequest(const QString &iApi,
                                const HandleResponse &handleResponse,
                                const Requester::Type iType,
                                const QByteArray &iData)
    {
        _progress->setHidden(false);
        _progress->setValue(0);

        QNetworkRequest request = _requester->createRequest(iApi);
        _manager->setCookieJar(_requester->getCookie());

        QNetworkReply *reply;
        switch (iType)
        {
            case Type::POST:
            {
                request.setRawHeader("Content-Length", QByteArray::number(iData.size()));
                reply = _manager->post(request, iData);
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
                    reply = _requester->sendCustomRequest(request, "DELETE", iData);
                break;
            }
            case Type::PATCH:
            {
                reply = _requester->sendCustomRequest(request, "PATCH", iData);
                break;
            }
            default:
                reply = Q_NULLPTR;
                Q_ASSERT(false);
        }

        connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(printProgress(qint64, qint64)));
        connect(reply, &QNetworkReply::finished, this, [this, iApi, reply, handleResponse]()
        {
            if (_requester->checkFinishRequest(reply))
            {
//                QMutexLocker locker(&mutex);
                _progress->setHidden(true);
                _json = _requester->parseReply(reply);
                _token.clear();
                _requester->getCookie()->clear();

                QVariant variantCookies = reply->header(QNetworkRequest::SetCookieHeader);
                if (!variantCookies.isNull())
                {
                    QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie>>(variantCookies);
                    for (const auto& cookie : cookies)
                    {
                        if (cookie.name() == "refreshToken")
                        {
                            _token = cookie.value();
                            if (!_token.isEmpty() && iApi != "logout")
                                _requester->getCookie()->setAllCookies({cookie});
                            else
                                _requester->getCookie()->clear();
                            break;
                        }
                    }
                }

                if (handleResponse)
                        handleResponse(true, reply->errorString());
                else
                    response(true);
            }
            else
            {
                if (handleResponse)
                        handleResponse(false, reply->errorString());
                else
                    response(false);
            }

            reply->close();
            reply->deleteLater();
            delete reply;
        });
    }
}
