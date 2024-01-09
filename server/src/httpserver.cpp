#include "httpserver.h"
#include "database.h"
#include "authenticationservice.h"
#include "server.h"

#include <QHttpServer>
#include <QQueue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#define SERVER_HOSTNAME "127.0.0.1" // Хост
#define SERVER_PORT      5433       // Порт

#define INFO(object, str) qInfo() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;
#define WARNING(object, str) qWarning() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;
#define CRITICAL(object, str) qCritical() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;

extern Server::DataBase *db;

namespace Server
{
    struct Tree
    {
        Tree() noexcept = default;
        ~Tree() noexcept = default;

        Tree(const Tree& other) noexcept
        {
            *this = other;
        }

        explicit Tree(Tree&& other) noexcept
        {
            *this = std::move(other);
        }

        Tree& operator=(Tree&& other) noexcept
        {
            table = std::move(other.table);
            id = std::exchange(other.id, 0);
            column = std::move(other.column);
            value = std::move(other.value);
            return *this;
        }

        Tree& operator=(const Tree& other) noexcept
        {
            table = other.table;
            id = other.id;
            column = other.column;
            value = other.value;
            return *this;
        }

        QByteArray table;  // Название таблицы в БД
        qint64 id;         // id пользователя в БД
        QByteArray column; // Название столбца в БД
        QVariant value;    // Значение в поле БД
    };

    class ICallback
    {
    public:
        virtual QHttpServerResponse operator()() = 0;
    };

    template<class TClass, class TCallBack, typename TArgs>
    struct AbstractCallback : public ICallback
    {
    public:
        AbstractCallback(TClass& iClass, TCallBack& iCallback, TArgs&& iArgs) :
        _class(iClass),
        _callback(iCallback),
        _args(iArgs)
        {

        }
        virtual QHttpServerResponse operator()() override
        {
            if (_callback)
                return (_class.*(_callback))((std::forward<TArgs>(_args)));
            else
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }
    protected:
        TClass _class;
        TCallBack _callback;
        TArgs _args;
    };

    template <class TClass, class TCallBack, class... TArgs>
    class Callback : public ICallback
    {
        using _CallBack = std::function<QHttpServerResponse()>;
    public:
        Callback(TClass& iClass, const TCallBack& iCallback, TArgs&&... iArgs)
        {
            _callback = std::bind(iCallback, &iClass, std::forward<TArgs>(iArgs)...);
        }

        QHttpServerResponse operator()() override
        {
            if (_callback)
                return _callback();
            else
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

    private:
        _CallBack _callback;
    };

    class HttpServer::HttpServerImpl
    {
        using StatusCode = QHttpServerResponse::StatusCode;
        template <class TCallBack>
        friend class AuthorizationService;
    public:
        HttpServerImpl() :
          _host(SERVER_HOSTNAME),
          _port(SERVER_PORT)
        {

        }

        void _start();
    private:
        bool _authentication(const QHttpServerRequest &iRequest, QByteArray* oData = nullptr);
        QHttpServerResponse _login();
        QHttpServerResponse _logout();
        QHttpServerResponse _showPersonalData();
        QHttpServerResponse _showDatabase();
        QHttpServerResponse _updatePersonalData(QQueue<Tree>& iTrees);
        QHttpServerResponse _updateDatabase(QQueue<Tree>& iTrees, QHttpServerRequest::Method iMethod);

    private:
        AuthenticationService _authenticationService;

    private:
        QString _host;
        int _port;
        QHttpServer _server;
    };

    template <class TCallBack>
    class AuthorizationService
    {
        using StatusCode = QHttpServerResponse::StatusCode;
    public:
        AuthorizationService(HttpServer::HttpServerImpl &iServer, const QHttpServerRequest &iRequest, const TCallBack& iCallback) :
            _authenticationService(iServer._authenticationService),
            _request(iRequest)
        {
            if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(), TCallBack>)
            {
                 _callback.reset(new Callback(iServer, iCallback));
            }
            else if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(QQueue<Tree>&), TCallBack>)
            {
                _callback.reset(new Callback(iServer, iCallback, std::ref(_trees)));
            }
            else if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(QQueue<Tree>&, QHttpServerRequest::Method), TCallBack>)
            {
                _callback.reset(new Callback(iServer, iCallback, std::ref(_trees), _request.method()));
            }
        }

        QHttpServerResponse checkAccess(const QByteArray &iTable);

    private:
        bool parseObject(const QJsonValue& iTable);
        bool parseData();
        bool _checkAccess(const QByteArray &iTable);

    private:
        QScopedPointer<ICallback> _callback;
        AuthenticationService& _authenticationService;
        const QHttpServerRequest &_request;
        QQueue<Tree> _trees;
    };

    bool HttpServer::HttpServerImpl::_authentication(const QHttpServerRequest &iRequest, QByteArray* oData)
    {
        QByteArray authentication = iRequest.value("authorization").simplified();
        if (!authentication.isNull())
        {
            if (authentication.startsWith("Basic"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(6));
                QList<QByteArray> parts = token.split(':');
                if (parts.size() == 2)
                {
                    QByteArray userName = parts[0];
                    QByteArray password = parts[1];
                    QString id, role;
                    if (oData && db->authentication(userName, password, id, role, *oData))
                    {
                        INFO(_authenticationService, "Аутентификации пройдена");
                        _authenticationService.setID(id.toULongLong());
                        _authenticationService.setUserName(userName);
                        _authenticationService.setRole(role);
                        return true;
                    }
                }
            }
            else if (authentication.startsWith("Bearer"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(7));
                if (_authenticationService.checkAuthentication(token))
                {
                    INFO(_authenticationService, "Аутентификации пройдена");
                    if (oData)
                    {
                        if (db->getPeronalData(_authenticationService.getID(), _authenticationService.getRole(), _authenticationService.getUserName(), *oData))
                            return true;
                    }
                    else
                        return true;
                }
            }
        }

        CRITICAL(_authenticationService, "Аутентификации не пройдена");
        return false;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_login()
    {
        for (const auto& request: QList<QByteArray>{
                                   "DROP TABLE IF EXISTS tmp;",
                                   "SELECT * INTO tmp FROM " + Employee::permissionTable().toUtf8() + ";",
                                   "DELETE FROM tmp WHERE tmp.role_id NOT IN (SELECT id FROM role WHERE role.code = '" + _authenticationService.getRole() +"'); ",
                                   "ALTER TABLE tmp DROP COLUMN id, DROP COLUMN role_id;",
                                   "SELECT * FROM tmp;"})
        {


            if (!db->sendRequest(request))
            {
                CRITICAL(_authenticationService, "Неверный запрос");
                return QHttpServerResponse(StatusCode::BadRequest);
            }
        }

        QByteArray data1;
        if (!db->sendRequest("SELECT * FROM tmp;", data1, Employee::permissionTable().toUtf8()))
        {
            CRITICAL(_authenticationService, "Неверный запрос");
            return QHttpServerResponse(StatusCode::BadRequest);
        }

        QByteArray data2;
        if (!db->sendRequest("SELECT "
                             "p.role, "
                             "p.surname, "
                             "p.name, "
                             "p.patronymic, "
                             "p.sex, "
                             "p.date_of_birth, "
                             "p.passport, "
                             "p.phone, "
                             "p.email, "
                             "p.date_of_hiring, "
                             "p.working_hours, "
                             "p.salary, "
                             "p.password FROM " + Employee::personalDataPermissionTable().toUtf8() + " AS p "
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService.getRole() + "';", data2, Employee::personalDataPermissionTable().toUtf8()))
        {
            CRITICAL(_authenticationService, "Неверный запрос");
            return QHttpServerResponse(StatusCode::BadRequest);
        }

        INFO(_authenticationService, "Получение прав");
        QHttpServerResponse response(std::move(data1) + std::move(data2));
        response.addHeader("Content-Type", "application/json");
        return response;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_logout()
    {
        _authenticationService.logout();
        return QHttpServerResponse(StatusCode::ResetContent);
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_showPersonalData()
    {
        QByteArray data;
        if (!db->getPeronalData(_authenticationService.getID(), _authenticationService.getRole(), _authenticationService.getUserName(), data))
        {
            CRITICAL(_authenticationService, "Неверный запрос");
            return QHttpServerResponse(StatusCode::BadRequest);
        }

        QHttpServerResponse response(std::move(data));
        response.addHeader("Content-Type", "application/json");
        return response;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_showDatabase()
    {
        QByteArray data1;
        if (!db->sendRequest("SELECT employee.id, "
                            "role.name as role, "
                            "employee.surname, "
                            "employee.name, "
                            "employee.patronymic, "
                            "employee.sex, "
                            "employee.date_of_birth, "
                            "employee.passport, "
                            "employee.phone, "
                            "employee.email, "
                            "employee.date_of_hiring, "
                            "employee.working_hours, "
                            "employee.salary, "
                            "employee.password "
                            "FROM employee LEFT JOIN role ON employee.role_id = role.id "
                            "WHERE employee.id != " + QByteArray::number(_authenticationService.getID()) + ";", data1, Employee::employeeTable().toUtf8()))
        {
            CRITICAL(_authenticationService, "Неверный запрос");
            return QHttpServerResponse(StatusCode::BadRequest);
        }

        QByteArray data2;
        if (!db->sendRequest("SELECT "
                             "p.role, "
                             "p.surname, "
                             "p.name, "
                             "p.patronymic, "
                             "p.sex, "
                             "p.date_of_birth, "
                             "p.passport, "
                             "p.phone, "
                             "p.email, "
                             "p.date_of_hiring, "
                             "p.working_hours, "
                             "p.salary, "
                             "p.password FROM " + Employee::databasePermissionTable().toUtf8() + " AS p "
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService.getRole() + "';", data2, Employee::databasePermissionTable().toUtf8()))
        {
            CRITICAL(_authenticationService, "Неверный запрос");
            return QHttpServerResponse(StatusCode::BadRequest);
        }

        INFO(_authenticationService, "Показать БД");
        QHttpServerResponse response(std::move(data1) + std::move(data2));
        response.addHeader("Content-Type", "application/json");
        return response;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_updatePersonalData(QQueue<Tree>& iTrees)
    {
        while (!iTrees.empty())
        {
            Tree tree = iTrees.front();
            iTrees.pop_front();

            if (!db->checkFieldOnDuplicate(tree.column, tree.value))
            {
                CRITICAL(_authenticationService, "Неверный запрос");
                return QHttpServerResponse(StatusCode::Conflict);
            }

            if (!db->updateRecord(tree.id, tree.column, tree.value))
            {
                CRITICAL(_authenticationService, "Неверный запрос");
                return QHttpServerResponse(StatusCode::BadRequest);
            }

            if (tree.column == "id")
            {
                _authenticationService.setID(tree.value.toULongLong());
            }
            else if (tree.column == "role")
            {
                QByteArray data;
                if (!db->sendRequest("SELECT code FROM " + Employee::role().toUtf8() + " WHERE name = '" + tree.value.toString().toUtf8() +"'", data))
                {
                    CRITICAL(_authenticationService, "Неверный запрос");
                    return QHttpServerResponse(StatusCode::BadRequest);
                }

                const QString role = QJsonDocument::fromJson(data).object().value("code").toString();
                _authenticationService.setRole(std::move(role));
            }
            else if (tree.column == "email")
            {
                _authenticationService.setUserName(tree.value.toString());
            }
        }

        INFO(_authenticationService, "Данные пользователя успешно обновлены");
        return QHttpServerResponse(StatusCode::Ok);
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_updateDatabase(QQueue<Tree>& iTrees, QHttpServerRequest::Method iMethod)
    {
        StatusCode statusCode = StatusCode::BadRequest;
        if (iMethod == QHttpServerRequest::Method::Post)
        {
            QHash<QString, QVariant> data;
            const auto fieldNames = Employee::getFieldNames();
            while (!iTrees.empty())
            {
                Tree tree = iTrees.front();
                iTrees.pop_front();

                auto fieldName = std::find_if(fieldNames.constBegin(), fieldNames.constEnd(), [&tree](const auto& fieldName)
                {
                    return fieldName.first == tree.column;
                });
                if (fieldName == fieldNames.constEnd())
                {
                    CRITICAL(_authenticationService, "Неверный запрос");
                    return QHttpServerResponse(StatusCode::BadRequest);
                }

                if (!db->checkFieldOnDuplicate(tree.column, tree.value))
                {
                    CRITICAL(_authenticationService, "Дублирование данных");
                    return QHttpServerResponse(StatusCode::Conflict);
                }

                data[tree.column] = tree.value;
            }

            if (!db->insertRecord(data))
            {
                CRITICAL(_authenticationService, "Неверный запрос");
                return QHttpServerResponse(StatusCode::BadRequest);
            }

            INFO(_authenticationService, "Пользователи успешно добавлены");
            statusCode = StatusCode::Created;
        }
        else if (iMethod == QHttpServerRequest::Method::Delete)
        {
            while (!iTrees.empty())
            {
                Tree tree = iTrees.front();
                iTrees.pop_front();

                if (!db->deleteRecord(tree.id))
                {
                    CRITICAL(_authenticationService, "Неверный запрос");
                    return QHttpServerResponse(StatusCode::BadRequest);
                }
            }

            INFO(_authenticationService, "Пользователи успешно удалены");
            statusCode = StatusCode::Ok;
        }
        else if (iMethod == QHttpServerRequest::Method::Patch)
        {
            while (!iTrees.empty())
            {
                Tree tree = iTrees.front();
                iTrees.pop_front();

                if (!db->checkFieldOnDuplicate(tree.column, tree.value))
                {
                    CRITICAL(_authenticationService, "Дублирование данных");
                    return QHttpServerResponse(StatusCode::Conflict);
                }

                if (!db->updateRecord(tree.id, tree.column, tree.value))
                {
                    CRITICAL(_authenticationService, "Неверный запрос");
                    return QHttpServerResponse(StatusCode::BadRequest);
                }
            }

            INFO(_authenticationService, "Данные успешно обновлены");
            statusCode = StatusCode::Ok;
        }

        return QHttpServerResponse(statusCode);
    }

    void HttpServer::HttpServerImpl::_start()
    {
        const auto sslPort = _server.listen(QHostAddress(_host), _port);
        if (!sslPort)
        {
            qWarning() << "Серверу не удалось прослушать порт";
            return;
        }

        INFO(_authenticationService, "Сервер запущен");
        _server.route("/login", [this](const QHttpServerRequest &request)
        {
            QByteArray data;
            if (!_authentication(request, &data))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            auto login = _login();
            if (login.statusCode() == StatusCode::BadRequest)
                return login;

            QHttpServerResponse response(data + login.data());
            response.addHeader("Content-Type", "application/json");
            return response;
        });

        _server.route("/logout", [this](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return _logout();
        });

        _server.route("/showPersonalData", [this](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            auto login = _login();
            if (login.statusCode() == StatusCode::BadRequest)
                return login;

            auto showPersonalData = _showPersonalData();
            if (showPersonalData.statusCode() == StatusCode::BadRequest)
                return showPersonalData;

            QHttpServerResponse response(login.data() + showPersonalData.data());
            response.addHeader("Content-Type", "application/json");
            return response;
        });

        _server.route("/showDatabase", [&](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return AuthorizationService(*this, request, &HttpServer::HttpServerImpl::_showDatabase).checkAccess(Employee::permissionTable().toUtf8());
        });

        _server.route("/updatePersonalData", [&](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return AuthorizationService(*this, request, &HttpServer::HttpServerImpl::_updatePersonalData).checkAccess(Employee::personalDataPermissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Post, [&](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return AuthorizationService(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).checkAccess(Employee::permissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Delete, [&](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return AuthorizationService(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).checkAccess(Employee::permissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Patch, [&](const QHttpServerRequest &request)
        {
            if (!_authentication(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return AuthorizationService(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).checkAccess(Employee::databasePermissionTable().toUtf8());
        });

        _server.afterRequest([this](QHttpServerResponse &&response)
        {
            if (response.statusCode() != StatusCode::ResetContent)
            {
                response.addHeader("Set-Cookie", _authenticationService.getAccessToken());
                response.addHeader("Set-Cookie", _authenticationService.getRefreshToken());
            }
            return std::move(response);
        });
    }

    template <class TCallBack>
    QHttpServerResponse AuthorizationService<TCallBack>::checkAccess(const QByteArray &iTable)
    {
        if (_checkAccess(iTable))
        {
            if (_callback)
                return (*_callback)();
            else
                return QHttpServerResponse(StatusCode::BadRequest);
        }

        CRITICAL(_authenticationService, "Нет прав доступа");
        return QHttpServerResponse("permission denied", QHttpServerResponse::StatusCode::Forbidden);
    }

    template <class TCallBack>
    bool AuthorizationService<TCallBack>::parseObject(const QJsonValue& iTable)
    {
        if (iTable.isObject())
        {
            const QJsonObject subobject = iTable.toObject();
            if (subobject.contains(Employee::id()) &&
                subobject.contains("column") &&
                subobject.contains("value"))
            {
                Tree tree;
                tree.table = Employee::employeeTable().toUtf8();
                tree.id = subobject.value(Employee::id()).toInteger();
                tree.column = subobject.value("column").toString().toUtf8();
                tree.value = subobject.value("value");

                if (tree.column == Employee::passport() ||
                    tree.column == Employee::phone())
                {
                    tree.value = tree.value.toByteArray().replace("-", "");
                }

                _trees.emplace_back(std::move(tree));
                return true;
            }
        }

        return false;
    };

    template <class TCallBack>
    bool AuthorizationService<TCallBack>::parseData()
    {
        if (_request.method() == QHttpServerRequest::Method::Get)
        {
            return true;
        }
        else
        {
            if (_request.body().isEmpty())
                return false;

            QJsonParseError parseError;
            QJsonDocument json = QJsonDocument::fromJson(_request.body(), &parseError);
            if (parseError.error != QJsonParseError::NoError)
                return false;

            else if (_request.method() == QHttpServerRequest::Method::Delete)
            {
                auto parseData = [this](const QJsonValue& iTable)->bool
                {
                    if (iTable.isObject())
                    {
                        const QJsonObject subobject = iTable.toObject();
                        if (subobject.contains(Employee::id()))
                        {
                            Tree tree;
                            tree.table = Employee::employeeTable().toUtf8();
                            tree.id = subobject.value(Employee::id()).toInt();
                            _trees.emplace_back(std::move(tree));
                            return true;
                        }
                    }

                    return false;
                };

                if (json.isObject())
                {
                    const QJsonObject object = json.object();
                    if (object.contains(Employee::employeeTable()))
                    {
                        auto table = object.value(Employee::employeeTable());
                        if (table.isObject())
                        {
                            return parseData(table);
                        }
                        else if (table.isArray())
                        {
                            for (const auto& subtable: table.toArray())
                            {
                                if (!parseData(subtable))
                                    return false;
                            }

                            return true;
                        }
                    }
                }
            }
            else if (_request.method() == QHttpServerRequest::Method::Post ||
                     _request.method() == QHttpServerRequest::Method::Patch)
            {
                if (json.isObject())
                {
                    const QJsonObject object = json.object();
                    if (object.contains(Employee::employeeTable()))
                    {
                        auto table = object.value(Employee::employeeTable());
                        if (table.isObject())
                        {
                            return parseObject(table);
                        }
                        else if (table.isArray())
                        {
                            for (const auto& subTable: table.toArray())
                            {
                                if (subTable.isObject())
                                {
                                    if (!parseObject(subTable))
                                        return false;
                                }
                                else if (subTable.isArray())
                                {
                                    for (const auto& sub2Table: subTable.toArray())
                                    {
                                        if (sub2Table.isObject())
                                        {
                                            if (!parseObject(sub2Table))
                                                return false;
                                        }
                                        else
                                        {
                                            return false;
                                        }
                                    }
                                }
                                else
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    template <class TCallBack>
    bool AuthorizationService<TCallBack>::_checkAccess(const QByteArray &iTable)
    {
        if (!parseData())
        {
            CRITICAL(_authenticationService, "Данные не распарсились");
            return false;
        }

        if (iTable == Employee::permissionTable())
        {
            QByteArray request;
            if (_request.method() == QHttpServerRequest::Method::Get)
            {
                request = "SELECT show_db FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _authenticationService.getRole() + "' AND show_db = true;";
            }
            else if (_request.method() == QHttpServerRequest::Method::Post)
            {
                request = "SELECT create_user FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _authenticationService.getRole() + "' AND create_user = true;";
            }
            else if (_request.method() == QHttpServerRequest::Method::Delete)
            {
                request = "SELECT delete_user FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _authenticationService.getRole() + "' AND delete_user = true;";
            }

            QByteArray data;
            if (!db->sendRequest(request, data))
            {
                CRITICAL(_authenticationService, "Нет прав доступа");
                return false;
            }
        }
        else if (iTable == Employee::personalDataPermissionTable() ||
                 iTable == Employee::databasePermissionTable())
        {
            if (_request.method() == QHttpServerRequest::Method::Patch)
            {
                for (const auto& tree : _trees)
                {
                    QByteArray request = "SELECT permission." + tree.column + " FROM " + iTable + " AS permission "
                                         "JOIN role ON permission.role_id = role.id WHERE role.code = '" + _authenticationService.getRole() + "' "
                                         "AND permission." + tree.column + " = 'write';";
                    QByteArray data;
                    if (!db->sendRequest(request, data))
                    {
                        CRITICAL(_authenticationService, "Нет прав доступа");
                        return false;
                    }
                }
            }
            else if (_request.method() == QHttpServerRequest::Method::Get)
            {
                for (const auto& tree : _trees)
                {
                    QByteArray request = "SELECT permission." + tree.column + " FROM " + iTable + " AS permission "
                                         "JOIN role ON permission.role_id = role.id WHERE role.code = '" + _authenticationService.getRole() + "' "
                                         "AND (permission." + tree.column + " = 'read' OR permission." + tree.column + " = 'write');";
                    QByteArray data;
                    if (!db->sendRequest(request, data))
                    {
                        CRITICAL(_authenticationService, "Нет прав доступа");
                        return false;
                    }
                }
            }
        }
        else
        {
            CRITICAL(_authenticationService, "Неизвестная ошибка");
            return false;
        }

        INFO(_authenticationService, "Доступ получен");
        return true;
    }


    HttpServer::HttpServer(QObject* parent) :
        QObject(parent),
        _server(new HttpServerImpl())
    {
        _server->_start();
    }

    HttpServer::~HttpServer()
    {
        qInfo() << "Отключение сервера";
    }

    void HttpServer::Start(QObject* parent)
    {
        qInfo() << "Запуск сервера";
        static HttpServer data(parent);
    }
}
