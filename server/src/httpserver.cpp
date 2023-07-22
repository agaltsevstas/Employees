#include "httpserver.h"
#include "database.h"
#include "authenticationservice.h"

#include <QHttpServer>
#include <QQueue>

#include <QJsonObject>

#define SERVER_HOSTNAME "127.0.0.1" // Хост
#define SERVER_PORT      5433       // Порт

extern Server::DataBase *db;

namespace Server
{
    struct Tree
    {
        enum class Type
        {
            POST,
            GET,
            PATCH,
            DELETE
        };

        Type type;
        QByteArray table;
        QByteArray id;
        QByteArray column;
        QByteArray value;
    };

    template <class Arg, class ... Args>
    constexpr int countArgs(const Arg& x, const Args& ... args)
    {
        return countArgs(args...) + 1;
    }

    template <typename R, typename T, typename ... Types>
    constexpr std::integral_constant<unsigned, sizeof ...(Types)> GetArgsCount(R(T::*)(Types ...))
    {
        return static_cast<int>(std::integral_constant<unsigned, sizeof ...(Types)>{});
    }

    class ICallback
    {
    public:
        virtual QHttpServerResponse operator()() = 0;
    };

    template<class TClass, class TCallBack, typename TArgs> struct AbstractCallback : public ICallback
    {
    public:
        AbstractCallback(TClass& iClass, TCallBack& iCallback, TArgs iArgs) :
        _class(iClass),
        _callback(iCallback),
        _args(iArgs)
        {

        }
        virtual QHttpServerResponse operator()() override
        {
            if (_callback)
                return (_class.*(_callback))(_args);
            else
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }
    protected:
        TClass _class;
        TCallBack _callback;
        TArgs _args;
    };

    template <class TClass, class TCallBack, class... TArgs> class Callback : public ICallback
    {
        typedef std::function<QHttpServerResponse()> _CallBack;
    public:
        Callback(TClass& iClass, const TCallBack& iCallback, const TArgs&... iArgs)
        {
            _callback = std::bind(iCallback, &iClass, iArgs...);
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

    template <class TCallBack> class Permission
    {
    public:
        Permission(HttpServer::HttpServerImpl &iServer, const QHttpServerRequest &iRequest, const TCallBack& iCallback) :
            _server(iServer),
            _request(iRequest)
        {
            if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(), TCallBack>)
            {
                 _callback.reset(new Callback(_server, iCallback));
            }
            else if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(QQueue<Tree>&), TCallBack>)
            {
                _callback.reset(new Callback(_server, iCallback, std::ref(_trees)));
            }
        }

        QHttpServerResponse check(const QByteArray &iTable);

    private:
        bool parseData();
        bool _check(const QByteArray &iTable);

    private:
        std::unique_ptr<ICallback> _callback;
        HttpServer::HttpServerImpl &_server;
        const QHttpServerRequest &_request;
        QQueue<Tree> _trees;
    };

    class HttpServer::HttpServerImpl
    {
        template <class TCallBack> friend class Permission;
    public:
        HttpServerImpl(QObject* parent = nullptr) :
          _host(SERVER_HOSTNAME),
          _port(SERVER_PORT),
          _authenticationService(new AuthenticationService(parent))
        {

        }

        void _start();
    private:
        bool _authorization(const QHttpServerRequest &iRequest, QByteArray* oData = nullptr);
        QHttpServerResponse _login();
        QHttpServerResponse _logout();
        QHttpServerResponse _showPersonalData();
        QHttpServerResponse _showDatabase();
        QHttpServerResponse _updatePersonalData(QQueue<Tree>& iTrees);
        QHttpServerResponse _updateDatabase(QQueue<Tree>& iTrees);

    private:
        AuthenticationService *_authenticationService;

    private:
        QString _host;
        int _port;
        QHttpServer _server;
    };

    bool HttpServer::HttpServerImpl::_authorization(const QHttpServerRequest &iRequest, QByteArray* oData)
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
                        _authenticationService->authentication(id, userName, role);
                        return true;
                    }
                }
            }
            else if (authentication.startsWith("Bearer"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(7));
                qint64 exp;
                if (_authenticationService->checkAuthentication(token, exp))
                    return true;
            }
        }

        return false;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_login()
    {
        for (const auto& request: QList<QByteArray>{
                                   "DROP TABLE IF EXISTS tmp;",
                                   "SELECT * INTO tmp FROM " + Employee::permissionTable().toUtf8() + ";",
                                   "DELETE FROM tmp WHERE tmp.role_id NOT IN (SELECT id FROM role WHERE role.code = '" + _authenticationService->getRole() +"'); ",
                                   "ALTER TABLE tmp DROP COLUMN id, DROP COLUMN role_id;",
                                   "SELECT * FROM tmp;"})
        {


            if (!db->sendRequest(request))
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        QByteArray data1;
        if (!db->sendRequest("SELECT * FROM tmp;", data1, Employee::permissionTable().toUtf8()))
            return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);

        QByteArray data2;
        if (!db->sendRequest("SELECT "
                             "role.code AS role, "
                             "surname.code AS surname, "
                             "name.code AS name, "
                             "patronymic.code AS patronymic, "
                             "sex.code AS sex, "
                             "date_of_birth.code AS date_of_birth, "
                             "passport.code AS passport, "
                             "phone.code AS phone, "
                             "email.code AS email, "
                             "date_of_hiring.code AS date_of_hiring, "
                             "working_hours.code AS working_hours, "
                             "salary.code AS salary, "
                             "password.code AS password "
                             "FROM " + Employee::personalDataPermissionTable().toUtf8() + " AS p "
                             "JOIN action AS role ON p.role = role.id "
                             "JOIN action AS surname ON p.surname = surname.id "
                             "JOIN action AS name ON p.name = name.id "
                             "JOIN action AS patronymic ON p.patronymic = patronymic.id "
                             "JOIN action AS sex ON p.sex = sex.id "
                             "JOIN action AS date_of_birth ON p.date_of_birth = date_of_birth.id "
                             "JOIN action AS passport ON p.passport = passport.id "
                             "JOIN action AS phone ON p.phone = phone.id "
                             "JOIN action AS email ON p.email = email.id "
                             "JOIN action AS date_of_hiring ON p.date_of_hiring = date_of_hiring.id "
                             "JOIN action AS working_hours ON p.working_hours = working_hours.id "
                             "JOIN action AS salary ON p.salary = salary.id "
                             "JOIN action AS password ON p.password = password.id "
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService->getRole() + "';", data2, Employee::personalDataPermissionTable().toUtf8()))
        {
            return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        QHttpServerResponse response(data1 + data2);
        response.addHeader("Content-Type", "application/json");
        return response;
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_logout()
    {
        _authenticationService->logout();
        return QHttpServerResponse(QHttpServerResponse::StatusCode::ResetContent);
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_showPersonalData()
    {
        const qint64 id = _authenticationService->getID();
        const QString username = _authenticationService->getUserName();
        const QString role = _authenticationService->getRole();

        QByteArray data;
        if (!db->getPeronalData(id, role, username, data))
            return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);

        return QHttpServerResponse(data);
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
                            "FROM employee LEFT JOIN role ON employee.role_id = role.id;", data1, Employee::employeeTable().toUtf8()))
        {
            return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        QByteArray data2;
        if (!db->sendRequest("SELECT "
                             "role.code AS role, "
                             "surname.code AS surname, "
                             "name.code AS name, "
                             "patronymic.code AS patronymic, "
                             "sex.code AS sex, "
                             "date_of_birth.code AS date_of_birth, "
                             "passport.code AS passport, "
                             "phone.code AS phone, "
                             "email.code AS email, "
                             "date_of_hiring.code AS date_of_hiring, "
                             "working_hours.code AS working_hours, "
                             "salary.code AS salary, "
                             "password.code AS password "
                             "FROM " + Employee::databasePermissionTable().toUtf8() + " AS p "
                             "JOIN action AS role ON p.role = role.id "
                             "JOIN action AS surname ON p.surname = surname.id "
                             "JOIN action AS name ON p.name = name.id "
                             "JOIN action AS patronymic ON p.patronymic = patronymic.id "
                             "JOIN action AS sex ON p.sex = sex.id "
                             "JOIN action AS date_of_birth ON p.date_of_birth = date_of_birth.id "
                             "JOIN action AS passport ON p.passport = passport.id "
                             "JOIN action AS phone ON p.phone = phone.id "
                             "JOIN action AS email ON p.email = email.id "
                             "JOIN action AS date_of_hiring ON p.date_of_hiring = date_of_hiring.id "
                             "JOIN action AS working_hours ON p.working_hours = working_hours.id "
                             "JOIN action AS salary ON p.salary = salary.id "
                             "JOIN action AS password ON p.password = password.id "
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService->getRole() + "';", data2, Employee::databasePermissionTable().toUtf8()))
        {
            return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        return QHttpServerResponse(data1 + data2);
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_updatePersonalData(QQueue<Tree>& iTrees)
    {
        while (!iTrees.empty())
        {
            Tree tree = iTrees.front();
            iTrees.pop_front();

            QByteArray request;
            if (tree.column == "role")
            {
                request = "UPDATE " + tree.table + " SET role_id = (select role.id FROM role WHERE role.name = '" + tree.value +  "') "
                          "FROM role WHERE employee.role_id = role.id "
                          "AND employee.id = " + tree.id + " "
                          "AND employee.email = '" + _authenticationService->getUserName() + "' "
                          "AND role.code = '" + _authenticationService->getRole() + "';";
            }
            else
            {
                request = "UPDATE " + tree.table + " SET " + tree.column + " = '" + tree.value +  "' "
                          "FROM role WHERE employee.role_id = role.id "
                          "AND employee.id = " + tree.id + " "
                          "AND employee.email = '" + _authenticationService->getUserName() + "' "
                          "AND role.code = '" + _authenticationService->getRole() + "';";
            }

            if (!db->sendRequest(request))
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
    }

    QHttpServerResponse HttpServer::HttpServerImpl::_updateDatabase(QQueue<Tree>& iTrees)
    {
        while (!iTrees.empty())
        {
            Tree tree = iTrees.front();
            iTrees.pop_front();

            if (tree.type == Tree::Type::POST)
            {
                if (!db->sendRequest("UPDATE " + tree.table + " SET " + tree.column + " = '" + tree.value +  "' WHERE id = " + tree.id +  ";"))
                    return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
            }
            else if (tree.type == Tree::Type::DELETE)
            {
                if (!db->removeRecord(tree.id.toLongLong()))
                    return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
            }
            else if (tree.type == Tree::Type::PATCH)
            {
                if (!db->sendRequest("UPDATE " + tree.table + " SET " + tree.column + " = '" + tree.value +  "' WHERE id = " + tree.id +  ";"))
                    return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
            }
        }

        return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
    }

    void HttpServer::HttpServerImpl::_start()
    {
        const auto sslPort = _server.listen(QHostAddress(_host), _port);
        if (!sslPort)
            qWarning() << "Server failed to listen on a port";

        _server.route("/login", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request)
        {
            QByteArray data;
            if (!_authorization(request, &data))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            auto login = _login();
            if (login.statusCode() == QHttpServerResponse::StatusCode::BadRequest)
                return login;

            QHttpServerResponse response(data + login.data());
            response.addHeader("Content-Type", "application/json");
            return response;
        });

        _server.route("/logout", [this](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            return _logout();
        });

        _server.route("/showPersonalData", [this](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            auto login = _login();
            if (login.statusCode() == QHttpServerResponse::StatusCode::BadRequest)
                return login;

            auto showPersonalData = _showPersonalData();
            if (showPersonalData.statusCode() == QHttpServerResponse::StatusCode::BadRequest)
                return showPersonalData;

            QHttpServerResponse response(login.data() + showPersonalData.data());
            response.addHeader("Content-Type", "application/json");
            return response;
        });

        _server.route("/showDatabase", [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_showDatabase).check("permission");
        });

        _server.route("/updatePersonalData", [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updatePersonalData).check("personal_data_permission");
        });

        _server.route("/updateDatabase", [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", QHttpServerResponder::StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).check("database_permission");
        });

        _server.afterRequest([this](QHttpServerResponse &&response)
        {
            if (response.statusCode() != QHttpServerResponse::StatusCode::ResetContent)
                response.addHeader("Set-Cookie", _authenticationService->getCookie());
            return std::move(response);
        });
    }

    template <class TCallBack>
    QHttpServerResponse Permission<TCallBack>::check(const QByteArray &iTable)
    {
        if (_check(iTable))
        {
            if (_callback)
                return (*_callback)();
            else
                return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
        }

        qDebug() << "permission denied";
        return QHttpServerResponse("permission denied", QHttpServerResponse::StatusCode::Forbidden);
    }

    template <class TCallBack>
    bool Permission<TCallBack>::parseData()
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
            {
                qWarning() << "Json parse error: " << parseError.errorString();
//                    _response.setStatus(403, parseError.errorString().toUtf8());
            }

            if (_request.method() == QHttpServerRequest::Method::Post)
            {
                return true;
            }
            else if (_request.method() == QHttpServerRequest::Method::Delete)
            {
                if (json.isObject())
                {
                    const QJsonObject object = json.object();
                    if (auto table = object.constFind(Employee::employeeTable()); table != object.constEnd())
                    {
                        if (table->isObject())
                        {
                            const QJsonObject subobject = table->toObject();
                            if (auto id = subobject.constFind(Employee::id()); id != subobject.constEnd())
                            {
                                Tree tree;
                                tree.type = Tree::Type::DELETE;
                                tree.table = Employee::employeeTable().toUtf8();
                                tree.id = id->toString().toUtf8();
                                _trees.push_back(tree);
                                return true;
                            }
                        }
                    }
                }
            }
            else if (_request.method() == QHttpServerRequest::Method::Patch)
            {
                auto parseObject = [this](const QJsonValue& iTable)->bool
                {
                    if (iTable.isObject())
                    {
                        const QJsonObject subobject = iTable.toObject();
                        auto id = subobject.constFind("id");
                        auto column = subobject.constFind("column");
                        auto value = subobject.constFind("value");
                        if (value != subobject.constEnd() &&
                            column != subobject.constEnd())
                        {
                            Tree tree;
                            tree.type = Tree::Type::PATCH;
                            tree.table = Employee::employeeTable().toUtf8();
                            /// Может быть updatePersonalData или updateDatabase
                            tree.id = (id != subobject.constEnd()) ? QByteArray::number(id->toInteger()) : QByteArray::number(_server._authenticationService->getID());
                            tree.column = column->toString().toUtf8();
                            tree.value = value->toString().toUtf8();
                            _trees.push_back(tree);
                            return true;
                        }
                    }

                    return false;
                };

                if (json.isObject())
                {
                    const QJsonObject object = json.object();
                    if (auto table = object.constFind(Employee::employeeTable()); table != object.constEnd())
                    {
                        if (table->isObject())
                        {
                            return parseObject(*table);
                        }
                        else if (table->isArray())
                        {
                            for (const auto& subtable: table->toArray())
                            {
                                if (!parseObject(subtable))
                                    return false;
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
    bool Permission<TCallBack>::_check(const QByteArray &iTable)
    {
        if (!parseData())
            return false;

        if (iTable == Employee::permissionTable())
        {
            QByteArray request = "SELECT show_db FROM " + iTable +
                      " JOIN role ON " + iTable + ".id = role.id "
                      "WHERE role.code = '" + _server._authenticationService->getRole() + "' AND show_db = true;";

            QByteArray data;
            if (!db->sendRequest(request, data))
            {
                qWarning() << "Ошибка прав доступа";
                return false;
            }
        }
        else if (iTable == Employee::personalDataPermissionTable() || iTable == Employee::databasePermissionTable())
        {
            for (const auto& tree : _trees)
            {
                QByteArray request = "SELECT permission." + tree.column + " FROM " + iTable + " AS permission "
                          "JOIN action ON permission." + tree.column + " = action.id "
                          "JOIN role ON permission.role_id = role.id WHERE role.code = '" + _server._authenticationService->getRole() + "' AND action.code = 'write'";

                QByteArray data;
                if (!db->sendRequest(request, data))
                {
                    qWarning() << "Ошибка прав доступа";
                    return false;
                }
            }
        }
        else
        {
            qDebug() << "...";
        }

        return true;
    }


    HttpServer::HttpServer(QObject* parent) :
        QObject(parent),
        _server(new HttpServerImpl(this))
    {
        _server->_start();
    }

    HttpServer::~HttpServer()
    {

    }

    void HttpServer::Start(QObject* parent)
    {
        static HttpServer data(parent);
    }
}
