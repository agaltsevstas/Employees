#include "qjsontablemodel.h"
#include "client.h"
#include "utils.h"

#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <Validator>

#include <ranges>


inline void swap(QJsonValueRef first, QJsonValueRef second) noexcept
{
    QJsonValue temp(std::move(first));
    first = QJsonValue(std::move(second));
    second = std::move(temp);
}

class JsonTableModel : public QAbstractTableModel
{
public: JsonTableModel(const QString& iHeader, const QStringList& iData, QObject* parent = nullptr) :
        QAbstractTableModel(parent),
       _header(iHeader),
       _data(iData)
    {
        setObjectName("jsonTableModel");
    }

private:
    inline QVariant headerData(int, Qt::Orientation, int) const noexcept override
    {
        return _header;
    }

    inline int columnCount(const QModelIndex&) const noexcept override
    {
        return 1;
    }

    inline int rowCount(const QModelIndex&) const noexcept override
    {
        return _data.size();
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        switch (role)
        {
            case Qt::FontRole:
//        case Qt::BackgroundRole:
//        case Qt::ForegroundRole:
            return QColor(Qt::red);
            [[fallthrough]];
            case Qt::EditRole:
            case Qt::DisplayRole:
            {
                return _data[index.row()];
            }
        }

        return {};
    }

private:
    QString _header;
    QStringList _data;
};


QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument& iDatabase, const QJsonDocument& iPermissions, QObject* parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setObjectName("jsonTableModel");
    setDatabase(iDatabase);
    setPermissions(iPermissions);
}

QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument& iDatabase, QObject* parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setObjectName("jsonTableModel");
    setDatabase(iDatabase);
}

void QJsonTableModel::setEditStrategy(EditStrategy iStrategy)
{
    if (iStrategy != _strategy)
    {
        _strategy = iStrategy;
        qInfo() << "Смена стратегии в БД";

        if (_strategy == OnFieldChange)
            submitAll();
    }
}

bool QJsonTableModel::setDatabase(const QJsonDocument& iDatabase) noexcept
{
    return setDatabase(iDatabase.array());
}

bool QJsonTableModel::setDatabase(const QJsonArray& iDatabase) noexcept
{
    if (iDatabase.isEmpty())
        return false;

    QJsonArray newDatabase = iDatabase;

    for (const auto i : std::views::iota(0, newDatabase.size()))
    {
        if (newDatabase[i].isObject())
        {
            QJsonObject object = newDatabase[i].toObject();
            if (object.contains(Client::Employee::passport()))
            {
                const QJsonValue passport = object.value(Client::Employee::passport());
                if (passport.isDouble())
                {
                    QLineEdit lineEdit;
                    lineEdit.setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport));
                    lineEdit.setText(QString::number(passport.toInteger()));

                    object.insert(Client::Employee::passport(), lineEdit.text());
                    newDatabase.replace(i, object);
                }
            }
            if (object.contains(Client::Employee::phone()))
            {
                const QJsonValue phone = object.value(Client::Employee::phone());
                if (phone.isDouble())
                {
                    QLineEdit lineEdit;
                    lineEdit.setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone));
                    lineEdit.setText(QString::number(phone.toInteger()));

                    object.insert(Client::Employee::phone(), lineEdit.text());
                    newDatabase.replace(i, object);
                }
            }
        }
    }

    qInfo() << "Установка БД";
    _recordsCache = _array = std::move(newDatabase);
    return true;
}

bool QJsonTableModel::setPermissions(const QJsonDocument& iPermissions)
{
    return setPermissions(iPermissions.object());
}

bool QJsonTableModel::setPermissions(const QJsonObject& iPermissions)
{
    if (iPermissions.isEmpty())
        return false;

    const auto fieldNames = Client::Employee::getFieldNames();
    _headers.append({{fieldNames.front().first, fieldNames.front().second}, false});
    for (const auto i : std::views::iota(1, fieldNames.size()))
    {
        const auto& [fieldNameEng, fieldNameRus] = fieldNames[i];

        if (iPermissions.contains(fieldNameEng))
        {
            _headers.append({{ fieldNameEng, fieldNameRus }, iPermissions.value(fieldNameEng) == "write"});
        }
    }

    qInfo() << "Установка доступа к БД";
    return true;
}

void QJsonTableModel::submitAll()
{
    if (!_recordsDeletedCache.empty())
    {
        sendDeleteRequest(QJsonDocument(QJsonObject{{_name, _recordsDeletedCache}}).toJson(), [&](const bool iResult, const QVariant& error)
        {
            if (iResult)
            {
                while (!_recordsDeletedCache.empty())
                {
                    if (_recordsDeletedCache.last().isObject())
                    {
                        const QJsonObject objectDeleted = _recordsDeletedCache.last().toObject();
                        if (objectDeleted.contains(Client::Employee::id()))
                        {
                            const auto id = objectDeleted.value(Client::Employee::id());
                            for (const auto i : std::views::iota(0, _recordsCache.size()))
                            {
                                if (_recordsCache[i].isObject())
                                {
                                    const QJsonObject objectCache = _recordsCache[i].toObject();
                                    if (objectCache.contains(Client::Employee::id()) &&
                                        objectCache.value(Client::Employee::id()) == id)
                                    {
                                        _recordsCache.removeAt(i);
                                        _array.removeAt(i);
                                        break;
                                    }
                                }
                            }

                            while (!_recordsUpdatedCache.empty())
                            {
                                if (_recordsUpdatedCache.last().isObject())
                                {
                                    const QJsonObject recordsUpdated = _recordsUpdatedCache.last().toObject();
                                    if (recordsUpdated.contains(Client::Employee::id()) &&
                                        recordsUpdated.value(Client::Employee::id()) == id)
                                    {
                                        _recordsUpdatedCache.pop_back();
                                    }
                                }
                            }

                            _recordsDeletedCache.pop_back();
                        }
                    }
                }

                qInfo() << "Пользователи БД успешно удалены";
            }
            else
            {
                while (!_recordsDeletedCache.empty())
                    _recordsDeletedCache.pop_back();

                QWidget* tableView = qobject_cast<QWidget*>(parent());
                if (!tableView)
                    return;

                qWarning() << "Ошибка: " << error;
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error.toString(), QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
            }

            emit layoutChanged();
        });
    }
    else
        qInfo() << "Пустые данные БД для удаления";

    if (!_recordsCreatedCache.empty())
    {
        sendCreateRequest(QJsonDocument(QJsonObject{{_name, _recordsCreatedCache}}).toJson(), [&](const bool iResult, const QVariant& error)
        {
            if (iResult)
            {
                qsizetype i = _array.size() - 1;
                while (!_recordsCreatedCache.empty())
                {
                    _recordsCreatedCache.pop_back();
                    _recordsCache.push_back(_array.at(i++));
                }

                qInfo() << "Пользователи БД успешно добавлены";
            }
            else
            {
                while (!_recordsCreatedCache.empty())
                {
                    _recordsCreatedCache.pop_back();
                    _array.pop_back();
                }

                QWidget* tableView = qobject_cast<QWidget*>(parent());
                if (!tableView)
                    return;

                qWarning() << "Ошибка: " << error;
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error.toString(), QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
            }

            emit layoutChanged();
        });
    }
    else
        qInfo() << "Пустые данные БД для создания";

    if (!_recordsUpdatedCache.empty())
    {
        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, _recordsUpdatedCache}}).toJson(), [&](const bool iResult, const QVariant& error)
        {
            if (iResult)
            {
                while (!_recordsUpdatedCache.empty())
                {
                    if (_recordsUpdatedCache.last().isObject())
                    {
                        const QJsonObject recordsUpdated = _recordsUpdatedCache.last().toObject();
                        if (recordsUpdated.contains(Client::Employee::id()) &&
                            recordsUpdated.contains("column") &&
                            recordsUpdated.contains("value"))
                        {
                            const auto id = recordsUpdated.value(Client::Employee::id());
                            const auto column = recordsUpdated.value("column").toString();
                            const auto value = recordsUpdated.value("value");

                            for (const auto i : std::views::iota(0, _recordsCache.size()))
                            {
                                if (_recordsCache[i].isObject())
                                {
                                    QJsonObject objectCache = _recordsCache[i].toObject();
                                    if (objectCache.contains(Client::Employee::id()) &&
                                        objectCache.contains(column) &&
                                        objectCache.value(Client::Employee::id()) == id)
                                    {
                                        objectCache[column] = std::move(value);
                                        _recordsCache[i] = std::move(objectCache);
                                        break;
                                    }
                                }
                            }

                            _recordsUpdatedCache.pop_back();
                        }
                    }
                }

                qInfo() << "Данные БД успешно обновлены";
            }
            else
            {
                while (!_recordsUpdatedCache.empty())
                {
                    if (_recordsUpdatedCache.last().isObject())
                    {
                        const QJsonObject recordsUpdated = _recordsUpdatedCache.last().toObject();
                        if (recordsUpdated.contains(Client::Employee::id()) &&
                            recordsUpdated.contains("column") &&
                            recordsUpdated.contains("value"))
                        {
                            const auto id = recordsUpdated.value(Client::Employee::id());
                            const auto column = recordsUpdated.value("column").toString();
                            const auto value = recordsUpdated.value("value");

                            for (const auto i : std::views::iota(0, _recordsCache.size()))
                            {
                                if (_recordsCache[i].isObject())
                                {
                                    QJsonObject objectCache = _recordsCache[i].toObject();
                                    if (objectCache.contains(Client::Employee::id()) &&
                                        objectCache.contains(column) &&
                                        objectCache.value(Client::Employee::id()) == id)
                                    {
                                        objectCache[column] = std::move(value);
                                        setJsonObject(i, std::move(objectCache));
                                        break;
                                    }
                                }
                            }

                            _recordsUpdatedCache.pop_back();
                        }
                    }
                }

                QWidget* tableView = qobject_cast<QWidget*>(parent());
                if (!tableView)
                    return;

                qWarning() << "Ошибка: " << error;
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error.toString(), QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
            }

            emit layoutChanged();
        });
    }
    else
        qInfo() << "Пустые данные БД для обновления";
}

bool QJsonTableModel::checkField(int row, int column, const QString& value) const
{
    if (column < 0)
        return false;

    qInfo() << "Проверка поля, строка: " << QString::number(row) << ", столбец: " << QString::number(column) << ", значение: " << value;
    const QString& field = _headers[column].first.first;
    QString message = value;

    if (auto tableView = qobject_cast<QWidget*>(parent()))
    {
        if (Client::Employee::checkField(field, message))
        {
            message = value; // нужно для проверки дубликата, checkField изменяет значение
            if (checkFieldOnDuplicate(row, column, message))
            {
                return true;
            }
            else
            {
                qWarning() << "Ошибка: " << message;
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
            }
        }
        else
        {
            qWarning() << "Ошибка: " << message;
            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
            QTimer::singleShot(1500, &warning, &QMessageBox::close);
            warning.exec();
        }
    }

    return false;
}

bool QJsonTableModel::checkField(const QModelIndex& index, const QString& iValue) const
{
    return checkField(index.row(), index.column(), iValue);
}

void QJsonTableModel::addRow(const QJsonObject& iEmployee)
{
    qInfo() << "Добавление пользователя: " << iEmployee;
    const qint64 newID = _array.last().toObject().value(Client::Employee::id()).toInteger() + 1;
    QJsonObject employee = iEmployee;
    employee.insert(Client::Employee::id(), newID);

    QJsonArray newEmployee;
    for (auto it = employee.constBegin(); it != employee.constEnd(); ++it)
    {
        QJsonObject record;
        record.insert(Client::Employee::id(), newID);
        record.insert("column", it.key());
        record.insert("value", it.value());

        newEmployee.push_back(std::move(record));
    }

    _recordsCreatedCache.push_back(std::move(newEmployee));
    _array.append(std::move(employee));
    emit layoutChanged();

    if (_strategy == OnFieldChange)
        submitAll();
};

void QJsonTableModel::deleteRow(int row)
{
    QJsonObject jsonObject = getJsonObject(row);
    const auto id = jsonObject[Client::Employee::id()];

    for (const auto i : std::views::iota(0, _recordsCreatedCache.size()))
    {
        if (_recordsCreatedCache[i].isArray())
        {
            QJsonArray recordCreated = _recordsCreatedCache[i].toArray();
            for (const auto j : std::views::iota(0, recordCreated.size()))
            {
                if (recordCreated[j].isObject())
                {
                    QJsonObject object = recordCreated[j].toObject();
                    if (object.contains(Client::Employee::id()) &&
                        object.contains("column") &&
                        object.value(Client::Employee::id()) == id)
                    {
                        _recordsCreatedCache.removeAt(i);
                        _array.removeAt(row);
                        emit layoutChanged();
                        return;
                    }
                }
            }
        }
    }

    for (const auto& recordDeleted : _recordsDeletedCache)
    {
        if (recordDeleted.isObject())
        {
            const QJsonObject object = recordDeleted.toObject();
            if (object.contains(Client::Employee::id()))
            {
                if (object.value(Client::Employee::id()) == id)
                {
                    return;
                }
            }
        }
    }

    qInfo() << "Удаление строки из БД >> " << QString::number(row);
    QJsonObject record;
    record.insert(Client::Employee::id(), id);
    _recordsDeletedCache.push_back(std::move(record));

    if (_strategy == OnFieldChange)
        submitAll();
}

void QJsonTableModel::restoreRow(int row)
{
    qInfo() << "Восстановление строки в БД >> " << QString::number(row);
    const qint64 id = getJsonObject(row)[Client::Employee::id()].toInteger();

    _recordsDeletedCache.erase(std::ranges::find_if(_recordsDeletedCache, [&](const auto& recordDeleted)
    {
        return recordDeleted.isObject() &&recordDeleted.toObject().contains(Client::Employee::id()) && recordDeleted.toObject().value(Client::Employee::id()) == id;
    }));
}

bool QJsonTableModel::canDeleteRow(int row)
{
    const qint64 id = getJsonObject(row)[Client::Employee::id()].toInteger();

    for (const auto& recordDeleted : _recordsDeletedCache)
    {
        if (recordDeleted.isObject())
        {
            const QJsonObject object = recordDeleted.toObject();
            if (object.contains(Client::Employee::id()))
            {
                if (object.value(Client::Employee::id()) == id)
                {
                    return false;
                }
            }
        }
    }

    qInfo() << "Проверка на удаление строки >> " << QString::number(row);
    return true;
}

bool QJsonTableModel::checkChanges() const noexcept
{
    return !_recordsCreatedCache.empty() || !_recordsDeletedCache.empty() || !_recordsUpdatedCache.empty();
}

QList<int> QJsonTableModel::search(const QString& iValue) const noexcept
{
    QList<int> hiddenIndices;
    QStringList values = iValue.split(" ");

    for (const auto i : std::views::iota(0, _array.count()))
    {
        if (_array[i].isObject())
        {
            qsizetype foundCount = 0;
            for (const auto& v : values)
            {
                const QJsonObject row = _array[i].toObject();
                for (const auto& column : row)
                {
                    if (column.isString())
                    {
                        QString value = column.toString().toLower();
                        if (value.contains(v.toLower()))
                        {
                            ++foundCount;
                            break;
                        }
                    }
                    else if (column.isDouble())
                    {
                        QString value = QString::number(column.toDouble()).toLower();
                        if (value.contains(v.toLower()))
                        {
                            ++foundCount;
                            break;
                        }
                    }
                }
            }

            if (foundCount != values.size())
                hiddenIndices.emplace_back(i);
        }
    }

    qInfo() << "Поиск слова в БД >> " +  iValue;
    return hiddenIndices;
}

bool QJsonTableModel::createEmail(int row)
{
    auto tableView = static_cast<QWidget*>(parent());
    if (!tableView)
        return false;

    if (_array.at(row).isObject())
    {
        QJsonObject rowObject = _array.at(row).toObject();
        if (rowObject.contains(Client::Employee::id()) &&
            rowObject.contains(Client::Employee::surname()) &&
            rowObject.contains(Client::Employee::name()) &&
            rowObject.contains(Client::Employee::patronymic()) &&
            rowObject.contains(Client::Employee::email()))
        {
            const QString surname = rowObject.take(Client::Employee::surname()).toString();
            const QString name = rowObject.take(Client::Employee::name()).toString();
            const QString patronymic = rowObject.take(Client::Employee::patronymic()).toString();
            const QString email = rowObject.take(Client::Employee::email()).toString();

            QString newEmail = Utils::CreateEmail(QVector<QString>{surname, name, patronymic});
            const int column = std::distance(_headers.constBegin(), std::ranges::find_if(_headers.constBegin(), _headers.constEnd(), [&](const auto& header)
            {
                return header.first.first == Client::Employee::email();
            }));

            while (true)
            {
                if (column > 0 && checkFieldOnDuplicate(row, column, newEmail))
                {
                    if (newEmail != email)
                        setData(index(row, column), newEmail);

                    break;
                }
                else
                {
                    while (true)
                    {
                        bool ok;
                        QString message = QInputDialog::getText(tableView,
                                                         tr("Предупреждение"),
                                                         tr("Данная почта уже существует, измените почту:"),
                                                         QLineEdit::Normal, newEmail, &ok);
                        if (ok)
                        {
                            if (Client::Employee::checkField(Client::Employee::email(), message))
                            {
                                break;
                            }
                            else
                            {
                                QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
                                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                                warning.exec();
                                qWarning() << "Ошибка: " << message;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
            }

            qInfo() << "Автосоздание почты в строке >> " << QString::number(row);
            return true;
        }
    }

    return false;
}

bool QJsonTableModel::checkFieldOnDuplicate(int row, int column, QString& iValue) const
{
    const QString& field = _headers[column].first.first;
    const QString& fieldName = _headers[column].first.second;

    if (field == Client::Employee::passport() ||
        field == Client::Employee::phone() ||
        field == Client::Employee::email())
    {
        for (const auto i : std::views::iota(0, _array.count()))
        {
            if (i != row)
            {
                if (_array[i].isObject())
                {
                    QJsonObject rowObject = _array[i].toObject();
                    if (rowObject.contains(field))
                    {
                        auto columnObject = rowObject.take(field);
                        if (columnObject.isString())
                        {
                            auto value = columnObject.toString();
                            if (value == iValue)
                            {
                                qWarning() << "Проверка на дубликат не пройдена, строка: " << QString::number(row) << ", столбец: " << QString::number(column) << ", значение: " << value;
                                iValue = "поле >> " + fieldName + value + " - повторяется";
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    qInfo() << "Проверка на дубликат пройдена, строка: " << QString::number(row) << ", столбец: " << QString::number(column) << ", значение: " << iValue << ", ";
    return true;
}

bool QJsonTableModel::checkRowOnDeleted(int row) const
{
    const auto id = getJsonObject(row)[Client::Employee::id()];

    for (const auto& recordDeleted : _recordsDeletedCache)
    {
        if (recordDeleted.isObject())
        {
            const QJsonObject object = recordDeleted.toObject();
            if (object.contains(Client::Employee::id()))
            {
                const QJsonValue recordId = object.value(Client::Employee::id());
                if (object.value(Client::Employee::id()) == id)
                {
                    qInfo() << "Проверка пройдена для удаленной строки >> " << QString::number(row);
                    return true;
                }
            }
        }
    }

    // qWarning() << "Проверка не пройдена для удаленной строки >> " << QString::number(row); // Лишняя информация
    return false;
}

bool QJsonTableModel::checkRowOnCreated(int row) const
{
    const auto id = getJsonObject(row)[Client::Employee::id()];

    for (const auto& recordCreated : _recordsCreatedCache)
    {
        if (recordCreated.isArray())
        {
            for (const auto& record : recordCreated.toArray())
            {
                if (record.isObject())
                {
                    const QJsonObject object = record.toObject();
                    if (object.contains(Client::Employee::id()))
                    {
                        if (object.value(Client::Employee::id()) == id)
                        {
                            qInfo() << "Проверка пройдена для созданной строки >> " << QString::number(row);
                            return true;
                        }
                    }
                }
            }
        }
    }

    // qWarning() << "Проверка не пройдена для созданной строки >> " << QString::number(row); // Лишняя информация
    return false;
}

bool QJsonTableModel::checkFieldOnUpdated(const QModelIndex& index) const
{
    const auto& [field, name] = _headers[index.column()].first;
    const auto id = getJsonObject(index.row())[Client::Employee::id()];
    const auto value = getJsonObject(index.row())[field];

    for (const auto& recordUpdated : _recordsUpdatedCache)
    {
        if (recordUpdated.isObject())
        {
            const QJsonObject object = recordUpdated.toObject();
            if (object.contains(Client::Employee::id()) &&
                object.contains("column") &&
                object.contains("value"))
            {
                if (object.value(Client::Employee::id()) == id &&
                    object.value("column") == field &&
                    object.value("value") == value)
                {
                    qInfo() << "Проверка пройдена для обновленного поля, строка: " << QString::number(index.row()) << ", столбец: " << QString::number(index.column()) << ", значение: " << value;
                    return true;
                }
            }
        }
    }

    // Лишняя информация
    // qInfo() << "Проверка не пройдена для обновленного поля, строка: " << QString::number(index.row()) << ", столбец: " << QString::number(index.column()) << ", значение: " << value;
    return false;
}

void QJsonTableModel::updateRecord(int row, const QString& iColumnName, const QString& iValue)
{
    const auto id = getJsonObject(row)[Client::Employee::id()].toInt();
    const auto value = _recordsCache[id].toObject().value(iColumnName).toString();

    QJsonObject record;
    record.insert(Client::Employee::id(), id);
    record.insert("column", iColumnName);
    record.insert("value", iValue);

    for (const auto i : std::views::iota(0, _recordsCreatedCache.size()))
    {
        if (_recordsCreatedCache[i].isArray())
        {
            QJsonArray recordCreated = _recordsCreatedCache[i].toArray();
            for (const auto j : std::views::iota(0, recordCreated.size()))
            {
                if (recordCreated[j].isObject())
                {
                    QJsonObject object = recordCreated[j].toObject();
                    if (object.contains(Client::Employee::id()) && object.contains("column") &&
                        object.value(Client::Employee::id()) == id && object.value("column") == iColumnName)
                    {
                        recordCreated.replace(j, std::move(record));
                        _recordsCreatedCache.replace(i, std::move(recordCreated));
                        return;
                    }
                }
            }
        }
    }

    bool found = false;
    for (const auto i : std::views::iota(0, _recordsUpdatedCache.size()))
    {
        if (_recordsUpdatedCache[i].isObject())
        {
            const QJsonObject object = _recordsUpdatedCache[i].toObject();
            if (object.contains(Client::Employee::id()) && object.contains("column") && object.contains("value"))
            {
                if (iValue == value)
                {
                    if (object.value(Client::Employee::id()) == id && object.value("column") == iColumnName)
                    {
                        found = true;
                        if (object.value("value") != iValue)
                        {
                            _recordsUpdatedCache.replace(i, std::move(record));
                            break;
                        }
                    }
                }
            }
        }
    }

    qInfo() << "Обновление поля, строка: " << QString::number(row) << ", столбец: " << iColumnName << ", значение: " << value;

    if (!found)
        _recordsUpdatedCache.push_back(std::move(record));

    if (_strategy == OnFieldChange)
        submitAll();
}

QAbstractItemModel *QJsonTableModel::relationModel(int column) const
{
    QString header = headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
    const auto fieldNames = Client::Employee::getFieldNames();
    auto fieldName = std::find_if(fieldNames.constBegin(), fieldNames.constEnd(), [header](const auto& fieldName)
        {
            return header == fieldName.second;
        });

    if (fieldName != fieldNames.constEnd())
    {
        if (fieldName->first == Client::Employee::role())
            return new JsonTableModel(header, Client::Employee::getRoles(), parent());
        else if (fieldName->first == Client::Employee::sex())
            return new JsonTableModel(header, Client::Employee::getSex(), parent());
    }

    return nullptr;
}

QVariant QJsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    switch (orientation)
    {
        case Qt::Horizontal:
            return _headers.value(section).first.second;
        case Qt::Vertical:
            return {};
        default:
            return {};
    }
}

int QJsonTableModel::rowCount(const QModelIndex&) const noexcept
{
    return _array.size();
}

int QJsonTableModel::columnCount(const QModelIndex&) const noexcept
{
    return _headers.size();
}

void QJsonTableModel::setJsonObject(int row, const QJsonObject& iJsonObject)
{
    _array[row] = std::move(iJsonObject);
}

QJsonObject QJsonTableModel::getJsonObject(int row) const
{
    return _array[row].toObject();
}

bool QJsonTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        QString message = value.toString();
        const QString& field = _headers[index.column()].first.first;
        QJsonObject jsonObject = getJsonObject(index.row());
        if (!message.isEmpty() && jsonObject.contains(field) && message != jsonObject[field].toString())
        {
            if (checkField(index, message))
            {
                jsonObject[field] = value.toJsonValue();
                setJsonObject(index.row(), jsonObject);
                updateRecord(index.row(), field, message);
                emit dataChanged(index, index);
                return true;
            }
        }
    }

    return false;
}

QVariant QJsonTableModel::data(const QModelIndex& index, int role) const
{
    switch (role)
    {
        case Qt::BackgroundRole:
        case Qt::ForegroundRole:
        {
            if (checkRowOnDeleted(index.row()))
                return (role == Qt::BackgroundRole) ? QColor(Qt::red) : QColor(Qt::white);
            if (checkRowOnCreated(index.row()))
                return (role == Qt::BackgroundRole) ? QColor(Qt::blue) : QColor(Qt::white);
            if (checkFieldOnUpdated(index))
                return (role == Qt::BackgroundRole) ? QColor(Qt::green) : QColor(Qt::white);
        }
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            const QJsonObject object = getJsonObject(index.row());
            const auto& [field, name] = _headers[index.column()].first;
            if (object.contains(field))
            {
                QJsonValue value = object[field];

                if (value.isString())
                {
                    return value.toString();
                }
                else if (value.isDouble())
                {
                    return QString::number(value.toDouble());
                }
                else
                {
                    Q_ASSERT(false);
                    return {};
                }
            }
        }
    }

    return {};
}

Qt::ItemFlags QJsonTableModel::flags(const QModelIndex& index) const
{
    if (index.isValid())
    {
        if (_headers[index.column()].second)
        {
            if (checkRowOnDeleted(index.row()))
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            else
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
        else
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return {};
}

bool QJsonTableModel::sortColumn(const QJsonValue& first, const QJsonValue& second, int column, Qt::SortOrder order) const
{
    auto value_1 = first.toObject()[_headers[column].first.first];
    auto value_2 = second.toObject()[_headers[column].first.first];
    if (value_1.isString())
    {
        if (order == Qt::SortOrder::AscendingOrder)
            return value_1.toString() < value_2.toString();
        else
            return value_1.toString() > value_2.toString();
    }
    else if (value_1.isDouble())
    {
        if (order == Qt::SortOrder::AscendingOrder)
            return value_1.toDouble() < value_2.toDouble();
        else
            return value_1.toDouble() > value_2.toDouble();
    }

    return false;
}

void QJsonTableModel::sort(int column, Qt::SortOrder order)
{
    qInfo() << "Сортировка в столбце" << column;
    std::sort(_array.begin(), _array.end(), [&](const auto& lhs, const auto& rhs)
    {
        return sortColumn(lhs, rhs, column, order);
    });

    emit layoutChanged();
}

bool QJsonTableModel::isSortColumn(int column) const
{
    qInfo() << "Проверка столбца на отсортированность " << column;
    return std::ranges::is_sorted(_array, [&](const auto& lhs, const auto& rhs) { return sortColumn(lhs, rhs, column); });
}
