#include "requester.h"
#include "cookie.h"
#include "session.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QEvent>
#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QProgressBar>
#include <QNetworkCookie>


namespace Client
{
    namespace
    {
        constexpr auto CLIENT_HOSTNAME = "127.0.0.1"; // Хост
        constexpr auto CLIENT_PORT = 5433; // Порт
        constinit QMutex mutex;

        QJsonDocument parseReply(QNetworkReply* iReply)
        {
            QByteArray replyData = iReply->readAll();
            if (replyData.isEmpty())
            {
                qWarning() << "Пришли пустые данные!";
                return {};
            }

            // TODO: Сделать вывод в отдельном потоке, иначе зависает из-за большого вывода информации
            //  qInfo() << "Получены данные: " + QString::fromUtf8(replyData);
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
    }

    struct HttpRequestEvent : public QEvent
    {
    private:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 1);
    public:
        HttpRequestEvent(Requester::Type iType,
                         const QString& iApi,
                         const QByteArray& iData = {},
                         const Requester::HandleResponse& iHandleResponse = Q_NULLPTR):
            QEvent(Type),
            type(iType),
            api(iApi),
            data(iData),
            handleResponse(iHandleResponse)
        {

        }
        Requester::Type type;
        const QString api;
        const QByteArray data;
        const Requester::HandleResponse handleResponse;
    };

    struct LoggerRequestEvent : public QEvent
    {
    private:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 2);
    public:
        LoggerRequestEvent(const QByteArray& iData):
            QEvent(Type),
            data(iData)
        {

        }

        const QByteArray data;
    };

    Request::Request(QObject* parent) :
        QObject(parent),
        _sslConfig(0),
        _manager(new QNetworkAccessManager(this))
    {

    }

    Request::~Request()
    {

    }

    void Request::sendRequest(const Type iType,
                              const QString& iApi,
                              const HandleResponse& iHandleResponse,
                              const QByteArray& iData)
    {
        QEvent* event = Q_NULLPTR;
        if (iApi == "log")
            event = new LoggerRequestEvent(iData);
        else
            event = new HttpRequestEvent(iType, iApi, iData, iHandleResponse);
        qApp->postEvent(this, event);
    }

    QSharedPointer<QNetworkRequest> Request::createRequest(const QString& iApi)
    {
        const QString url = QString("http://%1:%2//%3").arg(CLIENT_HOSTNAME).arg(CLIENT_PORT).arg(iApi);
        const QString uuid = QUuid::createUuid().toString().remove("{").remove("}");
        QSharedPointer<QNetworkRequest> request(new QNetworkRequest());

        request->setUrl(QUrl(url));
        request->setRawHeader("Content-Type", "application/json");
        if (Session::getSession().Cookie().isValid())
        {
            request->setRawHeader("Authorization", QString("Bearer %1").arg(_token.toUtf8().toBase64()).toLocal8Bit());
        }
        else
        {
            request->setRawHeader("Authorization", QString("Basic %1").arg(_token.toUtf8().toBase64()).toLocal8Bit());
        }

        request->setRawHeader("X-Request-ID", uuid.toUtf8());
        if (_sslConfig != Q_NULLPTR)
            request->setSslConfiguration(*_sslConfig);
        return request;
    }

    QNetworkReply* Request::sendCustomRequest(QSharedPointer<QNetworkRequest>& iRequest,
                                              const QString& iType,
                                              const QByteArray& iData)
    {
        iRequest->setRawHeader("HTTP", iType.toUtf8());
        QBuffer *buff = new QBuffer;
        buff->setData(iData);
        buff->open(QIODevice::ReadOnly);
        QNetworkReply* reply = _manager->sendCustomRequest(*iRequest, iType.toUtf8(), buff);
        buff->setParent(reply);
        return reply;
    }

    /// Отправка запроса и получение ответа в другом потоке
    void Request::customEvent(QEvent* iEvent)
    {
        if (auto event = dynamic_cast<HttpRequestEvent*>(iEvent); event)
        {
            mutex.lock();
            QSharedPointer<QNetworkRequest> request = createRequest(event->api);
            QNetworkReply *reply = Q_NULLPTR;
            switch (event->type)
            {
                case Type::GET:
                {
                    reply = _manager->get(*request);
                    break;
                }
                case POST:
                {
                    request->setRawHeader("Content-Length", QByteArray::number(event->data.size()));
                    reply = _manager->post(*request, event->data);
                    break;
                }
                case Type::PATCH:
                {
                    request->setRawHeader("Content-Length", QByteArray::number(event->data.size()));
                    reply = sendCustomRequest(request, "PATCH", event->data);
                    break;
                }
                case Type::DELETE:
                {
                    if (event->data.isEmpty())
                        reply = _manager->deleteResource(*request);
                    else
                        reply = sendCustomRequest(request, "DELETE", event->data);
                    break;
                }
                default:
                    reply = Q_NULLPTR;
                    Q_ASSERT(false);
            }

            QtFuture::connect(reply, &QNetworkReply::finished).
            // Async запускает обработку reply, чтобы освободить поток для отправки данных Logger клиента на сервер
            then(QtFuture::Launch::Async, [reply]() -> QNetworkReply*
            {
                auto replyError = reply->error();
                if (replyError == QNetworkReply::NoError)
                {
                    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                    if ((code >= 200) && (code < 300))
                    {
                         qInfo() << "Ответ на запрос";
                    }
                }
                else
                    throw reply->errorString();

                return reply;
            }).
            then(parseReply). // return QJsonDocument
            then([this, reply, api = std::move(event->api), handleResponse = std::move(event->handleResponse)](const QJsonDocument& json) -> void
            {
                // _token.clear(); // Закоментировал для отправления log данных

                QVariant variantCookies = reply->header(QNetworkRequest::SetCookieHeader);
                if (!variantCookies.isNull())
                {
                    QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie>>(variantCookies);
                    for (const auto& cookie : cookies)
                    {
                        if (cookie.name() == "accessToken" ||
                            cookie.name() == "refreshToken")
                        {
                            Session::getSession().Cookie().addToken(cookie.name(), cookie.value());
                        }
                    }
                }

                if (api == "y")
                    Session::getSession().Cookie().clear();

                reply->close();
                reply->deleteLater();
                // delete reply; // необязательно после deleteLater

                emit finished(true, json, handleResponse);
            }).
            onFailed([this, handleResponse = std::move(event->handleResponse)](const QString& error) -> void
            {
                emit finished(false, error, handleResponse);
            });
        }
        else if (auto event = dynamic_cast<LoggerRequestEvent*>(iEvent); event)
        {
            QSharedPointer<QNetworkRequest> request = createRequest("log");
            request->setRawHeader("Content-Length", QByteArray::number(event->data.size()));
            _manager->post(*request, event->data); // Отправляет request без reply
        }
        else
            Q_ASSERT(0);
    }

    Reply::Reply(Requester* iRequester) :
        QObject(iRequester),
        _requester(*iRequester)
    {

    }

    Reply::~Reply()
    {

    }

    /// После обработки ответа в другом потоке, далее идет переброска ответа уже в главном потоке
    void Reply::finished(const bool iResult, const QVariant& iData, const Requester::HandleResponse& iHandleResponse)
    {
        if (iHandleResponse)
            iHandleResponse(iResult, iData);
        mutex.unlock();
    }

    Requester::Requester(QObject* parent) :
        QObject(parent),
        _request(new Request()), // Не передавать родителя, иначе будет в главном потоке работать
        _reply(new Reply(this)),
        _thread(new QThread(this))
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setHorizontalStretch(10);

        connect(_request, &Request::finished, _reply, &Reply::finished); // Выполнение в главном потоке
        // connect(_request, &Request::finished, _reply, &Reply::finished, Qt::DirectConnection); // Выполнение в другом потоке

        _request->moveToThread(_thread);
        _thread->start();
    }

    Requester::~Requester()
    {
        _thread->terminate();

        delete _request;
    }

    void Requester::setToken(const QString& iToken) noexcept
    {
        _request->_token = iToken;
    }

    const QString Requester::getToken()
    {
        if (QString token = Session::getSession().Cookie().getValidToken(); !token.isEmpty())
            return token;

        return _request->_token;
    }

    void Requester::getResource(const QString& iApi, const HandleResponse& iHandleResponse)
    {
        if (!Session::getSession().Cookie().isValid())
            emit logout();

        _request->setToken(getToken());
        _request->sendRequest(Requester::Type::GET, iApi, iHandleResponse);
    }

    void Requester::patchResource(const QString& iApi,
                                  const QByteArray& iData,
                                  const HandleResponse& iHandleResponse)
    {
        if (!Session::getSession().Cookie().isValid())
            emit logout();

        _request->setToken(getToken());
        _request->sendRequest(Requester::Type::PATCH, iApi, iHandleResponse, iData);
    }

    void Requester::postResource(const QString& iApi,
                                 const QByteArray& iData,
                                 const HandleResponse& iHandleResponse)
    {
        if (!Session::getSession().Cookie().isValid())
            emit logout();

        _request->setToken(getToken());
        _request->sendRequest(Requester::Type::POST, iApi, iHandleResponse, iData);
    }

    void Requester::deleteResource(const QString& iApi,
                                   const QByteArray& iData,
                                   const HandleResponse& iHandleResponse)
    {
        if (!Session::getSession().Cookie().isValid())
            emit logout();

        _request->setToken(getToken());
        _request->sendRequest(Requester::Type::DELETE, iApi, iHandleResponse, iData);
    }
}
