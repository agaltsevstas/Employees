#include "controller/authorizationcontroller.h"

#include "controller/authenticationservice.h"

#include "database.h"

extern Server::DataBase* db;

namespace Server
{
    template <class TFunction>
    class Permission
    {
        typedef std::function<void()> CallFunction;

    public:
        Permission(AuthorizationController &iController, HttpResponse &iResponse, TFunction iFunction) :
            _controller(iController),
            _response(iResponse)
        {
            _callFunction = std::bind(iFunction, &_controller, std::ref(_response));
        }

        void check(const QString &iTable)
        {
            if (_check(iTable))
            {
                _callFunction();
            }
            else
            {
                qDebug() << "permission denied";
                _response.setStatus(400, "permission denied");
            }
        }

    private:
        bool _check(const QString &iTable)
        {
            QByteArray request = "SELECT * FROM " + iTable.toUtf8() +
                                 " LEFT JOIN role ON " + iTable.toUtf8() + ".id = role.id "
                                 "WHERE role.code = '" + _controller._authenticationService->getRole().toUtf8() + "'";
            QByteArray data;
            if (!db->sendRequest(request, data))
            {
                qWarning() << "Ошибка прав доступа";
                return false;
            }

            if (iTable == "database_permission")
            {

            }
            else
            {
                qDebug() << "...";
            }

            return true;
        }

    private:
        CallFunction _callFunction;

        AuthorizationController &_controller;
        HttpResponse &_response;
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
                        iResponse.setCookie(_authenticationService->authentication(id, userName, role));
                        iResponse.setHeader("Content-Type", "application/json");
                        iResponse.write(data);
                        return true;
                    }
                }
            }
            else if (authentication.startsWith("Bearer"))
            {
                QByteArray token = QByteArray::fromBase64(authentication.mid(7));
                if (_authenticationService->checkAuthentication(token))
                    return true;
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
                                   "DELETE FROM tmp WHERE tmp.role_id NOT IN (SELECT id FROM role WHERE role.code = '" + _authenticationService->getRole().toUtf8() +"'); ",
                                   "ALTER TABLE tmp DROP COLUMN id, DROP COLUMN role_id; ",
                                   "SELECT * FROM tmp;"})
        {


            QByteArray data;
            if (!db->sendRequest(request, data, QByteArray::fromStdString(Employee::PermissionTable())))
                return;

            iResponse.write(data);
        }

        QByteArray data;
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
                             "JOIN role AS role_id ON p.role_id = role_id.id WHERE role_id.code = '" + _authenticationService->getRole().toUtf8() + "';", data, QByteArray::fromStdString(Employee::PersonalDataPermissionTable())))
            return;

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

    void AuthorizationController::showDatabase(HttpResponse &iResponse)
    {
        QByteArray data;
        if (db->sendRequest("SELECT employee.id, "
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
                            "FROM employee LEFT JOIN role ON employee.role_id = role.id;", data))
        {
            iResponse.setHeader("Content-Type", "application/json");
            iResponse.write(data, true);
        }
        else
        {
            iResponse.setStatus(401, "Bad Request");
        }
    }

    void AuthorizationController::service(HttpRequest& iRequest, HttpResponse& iResponse)
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
            else if (path.startsWith("/showDatabase"))
            {
                Permission(*this, iResponse, &AuthorizationController::showDatabase).check("database_permission");
            }
            else if (path.startsWith("/updateData"))
            {

            }
            else
            {
                qDebug() << "error request";
                iResponse.setStatus(400, "Bad Request");
            }
        }
    }
}
