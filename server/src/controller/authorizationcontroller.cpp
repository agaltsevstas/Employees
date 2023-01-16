#include "controller/authorizationcontroller.h"

#include "controller/authenticationservice.h"

#include "database.h"

#include <QTextCodec>

extern Server::DataBase* db;

namespace Server
{
    struct Tree
    {
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
        virtual void operator()() = 0;
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
        virtual void operator()() override { (_class.*(_callback))(_args); }
    protected:
        TClass _class;
        TCallBack _callback;
        TArgs _args;
    };

    template <class TClass, class TCallBack, class... TArgs> class Callback : public ICallback
    {
        typedef std::function<void()> _CallBack;
    public:
        Callback(TClass& iClass, const TCallBack& iCallback, const TArgs&... iArgs)
        {
            _callback = std::bind(iCallback, &iClass, iArgs...);
        }

        void operator()() override
        {
            _callback();
        }

    private:
        _CallBack _callback;
    };

    template <class TCallBack> class Permission
    {
    public:
        Permission(AuthorizationController &iController, HttpRequest &iRequest, HttpResponse &iResponse, const TCallBack& iCallback) :
            _controller(iController),
            _request(iRequest),
            _response(iResponse)
        {
            if constexpr (std::is_same_v<void(AuthorizationController::*)(HttpResponse&), TCallBack>)
            {
                 _callback.reset(new Callback(iController, iCallback, std::ref(_response)));
            }
            else if constexpr (std::is_same_v<void(AuthorizationController::*)(HttpResponse&, const Tree&), TCallBack>)
            {
                _callback.reset(new Callback(iController, iCallback, std::ref(_response), std::ref(_tree)));
            }
        }

        void check(const QByteArray &iTable)
        {
            if (_check(iTable))
            {
                (*_callback)();
            }
            else
            {
                qDebug() << "permission denied";
                _response.setStatus(403, "Forbidden Error");
            }
        }

    private:

        bool parseData()
        {
            if (_request.getMethod() == "GET")
            {
                return true;
            }
            else if (_request.getMethod() == "PATCH")
            {
                if (_request.getBody().isEmpty())
                    return false;

                QJsonParseError parseError;
                QJsonDocument json = QJsonDocument::fromJson(_request.getBody(), &parseError);
                if (parseError.error != QJsonParseError::NoError)
                {
                    qWarning() << "Json parse error: " << parseError.errorString();
                    _response.setStatus(403, parseError.errorString().toUtf8());
                }

                if (json.isObject())
                {
                    const QJsonObject object = json.object();
                    if (auto table = object.constFind("employee"); table != object.constEnd())
                    {
                        if (table->isObject())
                        {
                            const QJsonObject subobject = table->toObject();
                            auto id = subobject.constFind("id");
                            auto column = subobject.constFind("column");
                            auto value = subobject.constFind("value");
                            if (id != object.constEnd() && value != object.constEnd() && column != object.constEnd())
                            {
                                _tree.table = "employee";
                                _tree.id = QByteArray::number(id->toInteger());
                                _tree.column = column->toString().toUtf8();
                                _tree.value = value->toString().toUtf8();
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        bool _check(const QByteArray &iTable)
        {
            if (!parseData())
                return false;

            QByteArray request;

            if (iTable == "permission")
            {
                request = "SELECT show_db FROM " + iTable +
                          " JOIN role ON " + iTable + ".id = role.id "
                          "WHERE role.code = '" + _controller._authenticationService->getRole() + "' AND show_db = true;";
            }
            else if (iTable == "personal_data_permission" || iTable == "database_permission")
            {
                request = "SELECT " + _tree.column + " FROM " + iTable +
                          " JOIN action ON " + _tree.column + " = action.id"
                          " JOIN role ON role_id = role.id WHERE role.code = '" + _controller._authenticationService->getRole() + "' AND action.code = 'write'";
            }
            else
            {
                qDebug() << "...";
            }

            QByteArray data;
            if (!db->sendRequest(request, data))
            {
                qWarning() << "Ошибка прав доступа";
                return false;
            }

            return true;
        }

    private:
        std::unique_ptr<ICallback> _callback;
        AuthorizationController &_controller;
        HttpRequest &_request;
        HttpResponse &_response;
        Tree _tree;
    };

    AuthorizationController::AuthorizationController(QObject *parent) :
        HttpRequestHandler(parent),
        _authenticationService(new AuthenticationService(parent))
    {

    }

    bool AuthorizationController::authorization(HttpRequest &iRequest, HttpResponse &iResponse)
    {
        QByteArray authentication = iRequest.getHeader("Authorization");
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
                    QByteArray data;
                    if (db->authentication(userName, password, id, role, data))
                    {
                        QByteArray token;
                        qint64 exp;
                        _authenticationService->authentication(id, userName, role, token, exp);
                        iResponse.setCookie(stefanfrings::HttpCookie("refreshToken",
                                                                     token,
                                                                     exp / 1000,
                                                                     "/",
                                                                     "jwt",
                                                                     "",
                                                                     true,
                                                                     false,
                                                                     "Lax"));

                        iResponse.setHeader("Content-Type", "application/json");
                        iResponse.write(data);
                        return true;
                    }
                }
            }
            else if (authentication.startsWith("Bearer"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(7));
                qint64 exp;
                if (_authenticationService->checkAuthentication(token, exp))
                {
                    iResponse.setCookie(stefanfrings::HttpCookie("refreshToken",
                                                                 token,
                                                                 exp / 1000,
                                                                 "/",
                                                                 "jwt",
                                                                 "",
                                                                 true,
                                                                 false,
                                                                 "Lax"));

                    return true;
                }
            }
        }


        qInfo("User is not logged in");
        iResponse.setStatus(401, "Unauthorized");
        iResponse.setHeader("WWW-Authenticate", "Basic realm = Please login with any name and password");
        return false;
    }

    void AuthorizationController::login(HttpResponse &iResponse)
    {
        for (const auto& request: QList<QByteArray>{
                                   "DROP TABLE IF EXISTS tmp;",
                                   "SELECT * INTO tmp FROM " + QByteArray::fromStdString(Employee::PermissionTable()) + ";",
                                   "DELETE FROM tmp WHERE tmp.role_id NOT IN (SELECT id FROM role WHERE role.code = '" + _authenticationService->getRole() +"'); ",
                                   "ALTER TABLE tmp DROP COLUMN id, DROP COLUMN role_id; ",
                                   "SELECT * FROM tmp;"})
        {


            if (!db->sendRequest(request))
            {
                iResponse.setStatus(401, "Bad Request");
                return;
            }
        }

        QByteArray data;
        if (!db->sendRequest("SELECT * FROM tmp;", data, QByteArray::fromStdString(Employee::PermissionTable())))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.write(data);
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
                             "FROM " + QByteArray::fromStdString(Employee::PersonalDataPermissionTable()) + " AS p "
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
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService->getRole() + "';", data, QByteArray::fromStdString(Employee::PersonalDataPermissionTable())))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.write(data);
    }

    void AuthorizationController::logout(HttpResponse &iResponse)
    {
        _authenticationService->logout();
        iResponse.setCookie(stefanfrings::HttpCookie("refreshToken",
                                                    "",
                                                    0,
                                                    "/",
                                                    "jwt",
                                                    "",
                                                    true,
                                                    false,
                                                    "Lax"));
    }

    void AuthorizationController::showPersonalData(HttpResponse &iResponse)
    {
        const qint64 id = _authenticationService->getID();
        const QString username = _authenticationService->getUserName();
        const QString role = _authenticationService->getRole();

        QByteArray data;
        if (!db->getPeronalData(id, role, username, data))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.setHeader("Content-Type", "application/json");
        iResponse.write(data);
    }

    void AuthorizationController::showDatabase(HttpResponse &iResponse)
    {
        QByteArray data;
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
                            "FROM employee LEFT JOIN role ON employee.role_id = role.id;", data, QByteArray::fromStdString(Employee::EmployeeTable())))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.setHeader("Content-Type", "application/json");
        iResponse.write(data);
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
                             "FROM " + QByteArray::fromStdString(Employee::DatabasePermissionTable()) + " AS p "
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
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService->getRole() + "';", data, QByteArray::fromStdString(Employee::DatabasePermissionTable())))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.write(data, true);
    }

    void AuthorizationController::updatePersonalData(HttpResponse &iResponse, const Tree& iTree)
    {
        QByteArray data;
        if (!db->sendRequest("UPDATE " + iTree.table + " SET " + iTree.column + " = '" + iTree.value +  "' "
                             "FROM role WHERE employee.role_id = role.id "
                             "AND employee.id = " + iTree.id + " "
                             "AND employee.email = '" + _authenticationService->getUserName() + "@tradingcompany.ru' "
                             "AND role.code = '" + _authenticationService->getRole() + "';", data, QByteArray::fromStdString(Employee::EmployeeTable())))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.setHeader("Content-Type", "application/json");
        iResponse.write(data, true);
    }

    void AuthorizationController::updateDatabase(HttpResponse &iResponse, const Tree& iTree)
    {
        if (!db->sendRequest("UPDATE " + iTree.table + " SET " + iTree.column + " = '" + iTree.value +  "' WHERE id = " + iTree.id +  ";"))
        {
            iResponse.setStatus(401, "Bad Request");
            return;
        }

        iResponse.setStatus(200, "OK");
    }

    void AuthorizationController::service(HttpRequest &iRequest, HttpResponse &iResponse)
    {
        QByteArray path = iRequest.getPath();
        qDebug("RequestMapper: path=%s", path.data());

        if (authorization(iRequest, iResponse))
        {
            if (path.startsWith("/login"))
            {
                login(iResponse);
            }
            else if (path.startsWith("/logout"))
            {
                logout(iResponse);
            }
            else if (path.startsWith("/showPersonalData"))
            {
                login(iResponse);
                showPersonalData(iResponse);
            }
            else if (path.startsWith("/showDatabase"))
            {
                Permission(*this, iRequest, iResponse, &AuthorizationController::showDatabase).check("permission");
            }
            else if (path.startsWith("/updatePersonalData"))
            {
                Permission(*this, iRequest, iResponse, &AuthorizationController::updatePersonalData).check("personal_data_permission");
            }
            else if (path.startsWith("/updateDatabase"))
            {
                Permission(*this, iRequest, iResponse, &AuthorizationController::updateDatabase).check("database_permission");
            }
            else
            {
                qDebug() << "error request";
                iResponse.setStatus(400, "Bad Request");
            }
        }
    }
}
