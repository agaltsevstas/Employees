#include "personaldatatable.h"
#include "client.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QJsonArray>


QSizePolicy GetSizePolice()
{
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    return sizePolicy;
}

PersonalDataTable::PersonalDataTable(const QString& iName, const QJsonDocument &iData, const QJsonDocument &iPermissions,
                                     QGridLayout *iLayout, QGroupBox *iGroupBox, QWidget *parent) :
                                     QWidget(parent), _name(iName), _layout(iLayout), _groupBox(iGroupBox)
{
    if (iData.isEmpty() || iPermissions.isEmpty())
    {
        Q_ASSERT(false);
        return;
    }

    const auto fields = Client::Employee::getFileds();
    for (decltype(fields.size()) i = 0, I = fields.size(); i < I; ++i)
    {
        const auto& [field, name] = fields[i];
        const QJsonObject object_data = iData.object(); // обязательно нужно определить
        const QJsonObject object_permissions = iPermissions.object(); // обязательно нужно определить

        auto it_data = object_data.find(field);
        auto it_permissions = object_permissions.find(field);
        if (it_data != object_data.end() && it_permissions != object_permissions.end())
        {
            QLabel *label = new QLabel(field + "Label", _groupBox);
            label->setSizePolicy(GetSizePolice());
            label->setText(name);
            _layout->addWidget(label, i, 0, 1, 1);

            QLineEdit *lineEdit = new QLineEdit(field, _groupBox);
//            connect(lineEdit, &QLineEdit::editingFinished, this, &PersonalDataTable::update);
            if (field == "password")
            {
                lineEdit->setEchoMode(QLineEdit::Password);
            }

            if (it_data->isString())
            {
                lineEdit->setText(it_data.value().toString());
            }
            else if (it_data->isDouble())
            {
                lineEdit->setText(QString::number(it_data.value().toInteger()));
            }
            else
            {
                Q_ASSERT(false);
            }

            if (it_permissions->isString())
            {
                lineEdit->setEnabled(it_permissions->toString() == "write");
            }
            else
            {
                Q_ASSERT(false);
            }
//            sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
            lineEdit->setSizePolicy(GetSizePolice());
            _layout->addWidget(lineEdit, i, 1, 1, 1);
            _data.push_back({label,lineEdit});
        }
    }
}

PersonalDataTable::~PersonalDataTable() {}

void PersonalDataTable::setEditStrategy(EditStrategy iStrategy)
{
    if (iStrategy != _strategy)
    {
        _strategy = iStrategy;

        if (_strategy == OnFieldChange)
        {
            submitAll();
            _recordsCache.reset();
        }
        else if (_strategy == OnManualSubmit)
        {
            _recordsCache.reset(new QJsonArray());
        }
    }
}

void PersonalDataTable::update()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    auto found = std::find_if(std::begin(_data), std::end(_data), [&lineEdit](const auto& data) { return data.second == lineEdit; });
    if (found == std::end(_data))
    {
        Q_ASSERT(false);
        return;
    }

    const auto& [label, line] = *found;
    const QString& columnRus = label->text();
    QJsonObject record;
    auto columnEng = std::find_if(std::begin(Client::Employee::getFileds()), std::end(Client::Employee::getFileds()), [&columnRus](const auto& field)
        {
            return field.second == columnRus;
        })->first;
    record.insert("column", columnEng);
    record.insert("value", line->text());

    if (_strategy == OnFieldChange)
    {
        sendRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson());
    }
    else if (_strategy == OnManualSubmit)
    {
        bool found = false;
        for (decltype(_recordsCache->size()) i = 0, I = _recordsCache->size(); i < I; ++i)
        {
            if (_recordsCache->at(i).isObject())
            {
                const QJsonObject object = _recordsCache->at(i).toObject();
                if (object.contains("column") && object.contains("value"))
                {
                    if (object.value("column") == columnEng)
                    {
                        found = true;
                        if (object.value("value") != line->text())
                        {
                            _recordsCache->replace(i, record);
                            break;
                        }
                    }
                }
            }
        }

        if (!found)
            _recordsCache->push_back(record);
    }

    lineEdit->clearFocus();
    lineEdit->deselect();
}

void PersonalDataTable::submitAll()
{
    if (_recordsCache && !_recordsCache->empty())
    {
        sendRequest(QJsonDocument(QJsonObject{{_name, *_recordsCache}}).toJson());

        while(_recordsCache->count())
             _recordsCache->pop_back();
    }
    else
        qInfo() << "Пустые данные!";
}
