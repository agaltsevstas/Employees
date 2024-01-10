#ifndef REQUESTER_H
#define REQUESTER_H

#include <QJsonDocument>
#include <QMutex>


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

        using HandleResponse = std::function<void(const bool, const QString&)>;

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

        /*!
         * \brief Устновить токен (access/refresh)
         * \param iToken - Токен (access/refresh)
         */
        void setToken(const QString &iToken) { _token = iToken; }

        /*!
         * \brief Получить данные формата JSon
         * \return Данные формата QJsonDocument
         */
        const QJsonDocument getJson() const { return _json; }

        /*!
         * \brief Получить прогресс выполнения ответа на запрос
         * \return Прогресс ответа на запрос
         */
        QProgressBar *getProgressBar() const { return _progress.get(); }

    signals:
        /*!
         * \brief Сигнал о выходе из личного кабинета
         */
        void logout();

    private slots:
        void printProgress(qint64 bytesReceived, qint64 bytesTotal);

    private:
        const QString getToken();

    private:
        QString _token;
        QNetworkAccessManager *_manager;
        QJsonDocument _json;
        QScopedPointer<QProgressBar> _progress;
        QMutex _mutex;
        class RequesterImpl;
        QScopedPointer<RequesterImpl> _requester;
        friend class RequesterImpl;
    };
}

#endif // REQUESTER_H
