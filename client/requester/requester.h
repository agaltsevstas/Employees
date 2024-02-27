#ifndef REQUESTER_H
#define REQUESTER_H

#include <QJsonDocument>


class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QSslConfiguration;

namespace Client
{
    class Request;
    class Answer;


    class Requester final : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY_MOVE(Requester)
    public:

        using HandleResponse = std::function<void(const bool, const QVariant&)>;

        enum class Type
        {
            GET,
            POST,
            PATCH,
            DELETE
        };

    public:

        explicit Requester(QObject* parent = 0);
        ~Requester();

        void getResource(const QString& iApi,
                         const HandleResponse& iHandleResponse = Q_NULLPTR);
        void patchResource(const QString& iApi,
                           const QByteArray& iData,
                           const HandleResponse& iHandleResponse = Q_NULLPTR);
        void postResource(const QString& iApi,
                          const QByteArray& iData,
                          const HandleResponse& iHandleResponse = Q_NULLPTR);
        void deleteResource(const QString& iApi,
                            const QByteArray& iData,
                            const HandleResponse& iHandleResponse = Q_NULLPTR);

        /*!
         * \brief Устновить токен (access/refresh)
         * \param iToken - Токен (access/refresh)
         */
        void setToken(const QString& iToken) noexcept;

    Q_SIGNALS:
        /*!
         * \brief Сигнал о выходе из личного кабинета
         */
        void logout();

    private:
        const QString getToken();

    private:
        Request* _request;
        Answer* _answer;
        QThread* _thread;
        friend class Request;
        friend class Answer;
    };

    class Request final : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY_MOVE(Request)

        using HandleResponse = Requester::HandleResponse;
        using Type = Requester::Type;
        using enum Type;

    public:
        ~Request();

    private:
        Request(QObject* parent = 0);

        void sendRequest(const Type iType,
                         const QString& iApi,
                         const HandleResponse& iHandleResponse = Q_NULLPTR,
                         const QByteArray& iData = {});

        /*!
         * \brief Устновить токен (access/refresh)
         * \param iToken - Токен (access/refresh)
         */
        void setToken(const QString& iToken) noexcept { _token = iToken; }

        [[nodiscard]] QSharedPointer<QNetworkRequest> createRequest(const QString &iApi);

        [[nodiscard]] QNetworkReply* sendCustomRequest(QSharedPointer<QNetworkRequest>& iRequest,
                                                       const QString& iType,
                                                       const QByteArray& iData);

    private:
        void customEvent(QEvent *event) override;

    Q_SIGNALS:
        void finished(const bool iResult, const QVariant& iData, const HandleResponse& iHandleResponse);

    private:
        QSslConfiguration *_sslConfig;
        QNetworkAccessManager *_manager;
        QString _token;
        friend class Requester;
    };

    class Answer final : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY_MOVE(Answer)

    public:
        ~Answer();

    private:
        explicit Answer(Requester* iRequester);

    public Q_SLOTS:
        void replyFinished(const bool iResult, const QVariant& iData, const Requester::HandleResponse& iHandleResponse);

    private:
        Requester &_requester;
        friend class Requester;
    };
}

#endif // REQUESTER_H
