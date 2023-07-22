#include "qjsontablemodel.h"
#include "client.h"
#include "utils.h"

#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>


inline void swap(QJsonValueRef first, QJsonValueRef second) noexcept
{
    QJsonValue temp(first);
    first = QJsonValue(second);
    second = temp;
}

class JsonTableModel : public QAbstractTableModel
{
public: JsonTableModel(const QString &iHeader, const QStringList& iData, QObject *parent = nullptr) :
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

    inline int columnCount(const QModelIndex &) const noexcept override
    {
        return 1;
    }

    inline int rowCount(const QModelIndex &) const noexcept override
    {
        return _data.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
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


QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions, QObject *parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setObjectName("jsonTableModel");
    setDatabase(iDatabase);
    setPermissions(iPermissions);
}

QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, QObject *parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setObjectName("jsonTableModel");
    setDatabase(iDatabase);
}

void QJsonTableModel::setEditStrategy(EditStrategy iStrategy) noexcept
{
    if (iStrategy != _strategy)
    {
        _strategy = iStrategy;

        if (_strategy == OnFieldChange)
            submitAll();
    }
}

bool QJsonTableModel::setDatabase(const QJsonDocument &iDatabase) noexcept
{
    return setDatabase(iDatabase.array());
}

bool QJsonTableModel::setDatabase(const QJsonArray &iDatabase) noexcept
{
    if (iDatabase.isEmpty())
        return false;

    _array = iDatabase;
    return true;
}

bool QJsonTableModel::setPermissions(const QJsonDocument &iPermissions)
{
    return setPermissions(iPermissions.object());
}

bool QJsonTableModel::setPermissions(const QJsonObject &iPermissions)
{
    if (iPermissions.isEmpty())
        return false;

    const auto fieldNames = Client::Employee::getFieldNames();
    _headers.append({{fieldNames.front().first, fieldNames.front().second}, false});
    for (decltype(fieldNames.size()) i = 1, I = fieldNames.size(); i < I; ++i)
    {
        const auto& [fieldNameEng, fieldNameRus] = fieldNames[i];

        auto it_permissions = iPermissions.find(fieldNameEng);
        if (it_permissions != iPermissions.end())
        {
            _headers.append({{ fieldNameEng, fieldNameRus }, it_permissions->toString() == "write"});
        }
    }

    return true;
}

void QJsonTableModel::submitAll()
{
    QWidget* tableView = qobject_cast<QWidget*>(parent());
    if (!tableView)
        return;

    if (!_recordsCreatedCache.empty())
    {
        sendCreateRequest(QJsonDocument(QJsonObject{{_name, _recordsCreatedCache}}).toJson(), [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                while(!_recordsCreatedCache.empty())
                    _recordsCreatedCache.pop_back();
                qDebug() << "Пользователи успешно добавлены!";
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        });
    }
    else
        qInfo() << "Пустые данные для создания!";

    if (!_recordsDeletedCache.empty())
    {
        sendDeleteRequest(QJsonDocument(QJsonObject{{_name, _recordsDeletedCache}}).toJson(), [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                while (!_recordsDeletedCache.empty())
                     _recordsDeletedCache.pop_back();
                qDebug() << "Пользователи успешно удалены!";
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        });
    }
    else
        qInfo() << "Пустые данные для удаления!";

    if (!_recordsUpdatedCache.empty())
    {
        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, _recordsUpdatedCache}}).toJson(), [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                while (!_recordsUpdatedCache.empty())
                     _recordsUpdatedCache.pop_back();
                qDebug() << "Ваши данные успешно обновлены!";
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        });
    }
    else
        qInfo() << "Пустые данные для обновления!";
}

bool QJsonTableModel::checkField(int row, int column, const QString &value) const
{
    if (column < 0)
        return false;

    const QString& field = _headers[column].first.first;
    QString message = value;

    if (auto tableView = qobject_cast<QWidget*>(parent()))
    {
        if (Client::Employee::checkField(field, message))
        {
            if (checkFieldOnDuplicate(row, column, message))
            {
                return true;
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << message;
            }
        }
        else
        {
            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
            QTimer::singleShot(1500, &warning, &QMessageBox::close);
            warning.exec();
            qDebug() << "Ошибка: " << message;
        }
    }

    return false;
}

bool QJsonTableModel::checkField(const QModelIndex &index, const QString &value) const
{
    return checkField(index.row(), index.column(), value);
}

void QJsonTableModel::addRow(const QJsonObject &iUser)
{
    QWidget* tableView = qobject_cast<QWidget*>(parent());
    if (!tableView)
        return;

    const qint64 id = iUser["id"].toInteger();

    QJsonObject record;
    record.insert(Client::Employee::id(), id);

    if (_strategy == OnFieldChange)
    {
        sendCreateRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson(), [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                _array.push_back(iUser);
                qDebug() << "Ваши данные успешно добавлены!";
                emit layoutChanged();
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        });
    }
    else if (_strategy == OnManualSubmit)
    {
        _recordsCreatedCache.push_back(iUser);
    }
};

void QJsonTableModel::deleteRow(int row)
{
    QWidget* tableView = qobject_cast<QWidget*>(parent());
    if (!tableView)
        return;

    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInteger();

    QJsonObject record;
    record.insert("id", id);

    if (_strategy == OnFieldChange)
    {
        sendDeleteRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson(), [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                _array.removeAt(row);
                emit layoutChanged();
                qDebug() << "Ваши данные успешно удалены!";
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        });
    }
    else if (_strategy == OnManualSubmit)
    {
        bool found = false;
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

        if (!found)
            _recordsDeletedCache.push_back(record);
    }
}

void QJsonTableModel::restoreRow(int row)
{
    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInteger();

    _recordsDeletedCache.erase(std::find_if(_recordsDeletedCache.begin(), _recordsDeletedCache.end(), [&](const auto& recordDeleted)
    {
        return recordDeleted.isObject() &&recordDeleted.toObject().contains(Client::Employee::id()) && recordDeleted.toObject().value(Client::Employee::id()) == id;
    }));
}

bool QJsonTableModel::canDeleteRow(int row)
{
    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInteger();

    for (const auto& recordDeleted : _recordsDeletedCache)
    {
        if (recordDeleted.isObject())
        {
            const QJsonObject object = recordDeleted.toObject();
            if (object.contains("id"))
            {
                if (object.value("id") == id)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

QList<int> QJsonTableModel::valueSearch(const QString &iValue) const noexcept
{
    QList<int> hiddenIndices;
    QStringList values = iValue.split(" ");

    for (qsizetype i = 0, I = _array.count(); i < I; ++i)
    {
        if (_array.at(i).isObject())
        {
            qsizetype foundCount = 0;
            for (const auto &v : values)
            {
                const QJsonObject row = _array.at(i).toObject();
                for (const auto &column : row)
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
                hiddenIndices.push_back(i);
        }
    }

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
        if (rowObject.contains("id") &&
            rowObject.contains("surname") &&
            rowObject.contains("name") &&
            rowObject.contains("patronymic") &&
            rowObject.contains("email"))
        {
            const QString surname = rowObject.take(Client::Employee::surname()).toString();
            const QString name = rowObject.take(Client::Employee::name()).toString();
            const QString patronymic = rowObject.take(Client::Employee::patronymic()).toString();
            const QString email = rowObject.take(Client::Employee::email()).toString();

            QString newEmail = Utils::CreateEmail(QVector<QString>{surname, name, patronymic});
            const int column = std::distance(_headers.constBegin(), std::find_if(_headers.constBegin(), _headers.constEnd(), [&](const auto& header)
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
                        qDebug() << "Данная почта уже существует: " << newEmail;

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
                                qDebug() << "Ошибка: " << message;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

bool QJsonTableModel::checkFieldOnDuplicate(int row, int column, QString &iValue) const
{
    const QString& field = _headers[column].first.first;
    const QString& fieldName = _headers[column].first.second;

    if (field == "passport" || field == "phone" || field == "email")
    {
        qsizetype size = _array.count();
        for (decltype(size) i = 0; i < size; ++i)
        {
            if (i != row)
            {
                if (_array.at(i).isObject())
                {
                    QJsonObject row = _array.at(i).toObject();
                    if (row.contains(field))
                    {
                        auto column = row.take(field);
                        if (column.isString())
                        {
                            auto value = column.toString();
                            if (value == iValue)
                            {
                                iValue = "поле >> " + fieldName + value + " - повторяется";
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool QJsonTableModel::checkRowOnDeleted(int row) const
{
    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInt();

    for (const auto& recordDelete : _recordsDeletedCache)
    {
        if (recordDelete.isObject())
        {
            if (recordDelete.toObject().contains(Client::Employee::id()))
            {
                const QJsonValue recordId = recordDelete.toObject().take(Client::Employee::id());
                if (recordId.isDouble())
                {
                    if (recordId.toInt() == id)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void QJsonTableModel::updateRecord(int index, const QString &columnName, const QString &value)
{
    QWidget* tableView = qobject_cast<QWidget*>(parent());
    if (!tableView)
        return;

    QJsonObject record;
    record.insert("id", index);
    record.insert("column", columnName);
    record.insert("value", value);

    if (_strategy == OnFieldChange)
    {
        HandleResponse handleResponse = [&](const bool iResult, const QString &error)
        {
            if (iResult)
            {
                /// TODO :
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1500, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << error;
            }
        };

        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson(), handleResponse);
    }
    else if (_strategy == OnManualSubmit)
    {
        bool found = false;
        for (decltype(_recordsUpdatedCache.size()) i = 0, I = _recordsUpdatedCache.size(); i < I; ++i)
        {
            if (_recordsUpdatedCache.at(i).isObject())
            {
                const QJsonObject object = _recordsUpdatedCache.at(i).toObject();
                if (object.contains("id") && object.contains("column") && object.contains("value"))
                {
                    if (object.value("id") == index && object.value("column") == columnName)
                    {
                        found = true;
                        if (object.value("value") != value)
                        {
                            _recordsUpdatedCache.replace(i, record);
                            break;
                        }
                    }
                }
            }
        }

        if (!found)
            _recordsUpdatedCache.push_back(record);
    }
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
            //return section + 1;
            return {};
        default:
            return {};
    }
}

int QJsonTableModel::rowCount(const QModelIndex &) const noexcept
{
    return _array.size();
}

int QJsonTableModel::columnCount(const QModelIndex &) const noexcept
{
    return _headers.size();
}

void QJsonTableModel::setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject)
{
    _array[index.row()] = iJsonObject;
}

QJsonObject QJsonTableModel::getJsonObject(int row) const
{
    return _array[row].toObject();
}

bool QJsonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        QString message = value.toString();
        const QString& field = _headers[index.column()].first.first;
        QJsonObject jsonObject = getJsonObject(index.row());
        if (!message.isEmpty() && jsonObject.contains(field) && message != jsonObject[field])
        {
            if (checkField(index, message))
            {
                jsonObject[field] = value.toJsonValue();
                setJsonObject(index, jsonObject);
                updateRecord(jsonObject["id"].toInt(), field, message);
                emit dataChanged(index, index);
                return true;
            }
        }
    }

    return false;
}

QVariant QJsonTableModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::BackgroundRole:
        {
            if (checkRowOnDeleted(index.row()))
                return QColor(Qt::red);
        }
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            const QJsonObject obj = getJsonObject(index.row());
            const auto& [field, name] = _headers[index.column()].first;
            if (obj.contains(field))
            {
                QJsonValue value = obj[field];

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

Qt::ItemFlags QJsonTableModel::flags(const QModelIndex &index) const
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

    return {};
}

bool QJsonTableModel::sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order) const
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
    qDebug() << "Сортировка";
    std::sort(_array.begin(), _array.end(), [&](const auto &first, const auto &second)
    {
        return sortColumn(first, second, column, order);
    });

    emit layoutChanged();
}

bool QJsonTableModel::isSortColumn(int column) const
{
    return std::is_sorted(std::begin(_array), std::end(_array), [&](const auto &lhs, const auto &rhs) { return sortColumn(lhs, rhs, column); });
}
