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
        Tree() noexcept
        {

        }

        Tree(const Tree& other) noexcept :
        table(other.table),
        id(other.id),
        column(other.column),
        value(other.value)
        {

        }

        explicit Tree(Tree&& other) noexcept :
            table(std::move(other.table)),
            id(std::exchange(other.id, 0)),
            column(std::move(other.column)),
            value(std::move(other.value))
        {

        }

        Tree& operator=(Tree&& other) noexcept
        {
            qInfo() << "оператор перемещения : " << this;
            table = std::move(other.table);
            id = std::exchange(other.id, 0);;
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

        QByteArray table;
        qint64 id;
        QByteArray column;
        QVariant value;
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
        using _CallBack = std::function<QHttpServerResponse()>;
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
        using StatusCode = QHttpServerResponse::StatusCode;
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
            else if constexpr (std::is_same_v<QHttpServerResponse(HttpServer::HttpServerImpl::*)(QQueue<Tree>&, QHttpServerRequest::Method), TCallBack>)
            {
                _callback.reset(new Callback(_server, iCallback, std::ref(_trees), _request.method()));
            }
        }

        QHttpServerResponse check(const QByteArray &iTable);

    private:
        bool parseObject(const QJsonValue& iTable);
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
        using StatusCode = QHttpServerResponse::StatusCode;
        template <class TCallBack> friend class Permission;
    public:
        HttpServerImpl(QObject* parent = nullptr) :
          _host(SERVER_HOSTNAME),
          _port(SERVER_PORT),
          _authenticationService(parent)
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
        QHttpServerResponse _updateDatabase(QQueue<Tree>& iTrees, QHttpServerRequest::Method iMethod);

    private:
        AuthenticationService _authenticationService;

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
                        _authenticationService.authentication(id, userName, role);
                        return true;
                    }
                }
            }
            else if (authentication.startsWith("Bearer"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(7));
                qint64 exp;
                if (_authenticationService.checkAuthentication(token, exp))
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
                                   "DELETE FROM tmp WHERE tmp.role_id NOT IN (SELECT id FROM role WHERE role.code = '" + _authenticationService.getRole() +"'); ",
                                   "ALTER TABLE tmp DROP COLUMN id, DROP COLUMN role_id;",
                                   "SELECT * FROM tmp;"})
        {


            if (!db->sendRequest(request))
                return QHttpServerResponse(StatusCode::BadRequest);
        }

        QByteArray data1;
        if (!db->sendRequest("SELECT * FROM tmp;", data1, Employee::permissionTable().toUtf8()))
            return QHttpServerResponse(StatusCode::BadRequest);

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
            return QHttpServerResponse(StatusCode::BadRequest);
        }

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
            return QHttpServerResponse(StatusCode::BadRequest);

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
            return QHttpServerResponse(StatusCode::BadRequest);
        }

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
                return QHttpServerResponse(StatusCode::Conflict);

            if (!db->updateRecord(tree.id, tree.column, tree.value))
                return QHttpServerResponse(StatusCode::BadRequest);
        }

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
                    return QHttpServerResponse(StatusCode::BadRequest);

                if (!db->checkFieldOnDuplicate(tree.column, tree.value))
                    return QHttpServerResponse(StatusCode::Conflict);

                data[tree.column] = tree.value;
            }

            if (!db->insertRecord(data))
                return QHttpServerResponse(StatusCode::BadRequest);

            statusCode = StatusCode::Created;
        }
        else if (iMethod == QHttpServerRequest::Method::Delete)
        {
            while (!iTrees.empty())
            {
                Tree tree = iTrees.front();
                iTrees.pop_front();

                if (!db->deleteRecord(tree.id))
                    return QHttpServerResponse(StatusCode::BadRequest);
            }

            statusCode = StatusCode::Ok;
        }
        else if (iMethod == QHttpServerRequest::Method::Patch)
        {
            while (!iTrees.empty())
            {
                Tree tree = iTrees.front();
                iTrees.pop_front();

                if (!db->checkFieldOnDuplicate(tree.column, tree.value))
                    return QHttpServerResponse(StatusCode::Conflict);

                if (!db->updateRecord(tree.id, tree.column, tree.value))
                    return QHttpServerResponse(StatusCode::BadRequest);
            }

            statusCode = StatusCode::Ok;
        }

        return QHttpServerResponse(statusCode);
    }

    void HttpServer::HttpServerImpl::_start()
    {
        const auto sslPort = _server.listen(QHostAddress(_host), _port);
        if (!sslPort)
            qWarning() << "Server failed to listen on a port";

        _server.route("/login", [this](const QHttpServerRequest &request)
        {
            QByteArray data;
            if (!_authorization(request, &data))
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
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return _logout();
        });

        _server.route("/showPersonalData", [this](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
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
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_showDatabase).check(Employee::permissionTable().toUtf8());
        });

        _server.route("/updatePersonalData", [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updatePersonalData).check(Employee::personalDataPermissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Post, [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).check(Employee::permissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Delete, [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).check(Employee::permissionTable().toUtf8());
        });

        _server.route("/updateDatabase", QHttpServerRequest::Method::Patch, [&](const QHttpServerRequest &request)
        {
            if (!_authorization(request))
                return QHttpServerResponse("WWW-Authenticate", "Basic realm = Please login with any name and password", StatusCode::Unauthorized);

            return Permission(*this, request, &HttpServer::HttpServerImpl::_updateDatabase).check(Employee::databasePermissionTable().toUtf8());
        });

        _server.afterRequest([this](QHttpServerResponse &&response)
        {
            if (response.statusCode() != StatusCode::ResetContent)
                response.addHeader("Set-Cookie", _authenticationService.getCookie());
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
                return QHttpServerResponse(StatusCode::BadRequest);
        }

        qDebug() << "permission denied";
        return QHttpServerResponse("permission denied", QHttpServerResponse::StatusCode::Forbidden);
    }

    template <class TCallBack>
    bool Permission<TCallBack>::parseObject(const QJsonValue& iTable)
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
    bool Permission<TCallBack>::_check(const QByteArray &iTable)
    {
        if (!parseData())
            return false;

        if (iTable == Employee::permissionTable())
        {
            QByteArray request;
            if (_request.method() == QHttpServerRequest::Method::Get)
            {
                request = "SELECT show_db FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _server._authenticationService.getRole() + "' AND show_db = true;";
            }
            else if (_request.method() == QHttpServerRequest::Method::Post)
            {
                request = "SELECT create_user FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _server._authenticationService.getRole() + "' AND create_user = true;";
            }
            else if (_request.method() == QHttpServerRequest::Method::Delete)
            {
                request = "SELECT delete_user FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _server._authenticationService.getRole() + "' AND delete_user = true;";
            }

            QByteArray data;
            if (!db->sendRequest(request, data))
            {
                qWarning() << "Ошибка прав доступа";
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
                                         "JOIN role ON permission.role_id = role.id WHERE role.code = '" + _server._authenticationService.getRole() + "' "
                                         "AND permission." + tree.column + " = 'write';";
                    QByteArray data;
                    if (!db->sendRequest(request, data))
                    {
                        qWarning() << "Ошибка прав доступа";
                        return false;
                    }
                }
            }
            else if (_request.method() == QHttpServerRequest::Method::Get)
            {
                for (const auto& tree : _trees)
                {
                    QByteArray request = "SELECT permission." + tree.column + " FROM " + iTable + " AS permission "
                                         "JOIN role ON permission.role_id = role.id WHERE role.code = '" + _server._authenticationService.getRole() + "' "
                                         "AND (permission." + tree.column + " = 'read' OR permission." + tree.column + " = 'write');";
                    QByteArray data;
                    if (!db->sendRequest(request, data))
                    {
                        qWarning() << "Ошибка прав доступа";
                        return false;
                    }
                }
            }
        }
        else
        {
            qDebug() << "...";
            return false;
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
