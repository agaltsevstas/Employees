#include "table.h"
#include "table_p.h"
#include "ui_table.h"
#include "client.h"
#include "requester.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QUInt64Validator>


namespace Client
{
    QSizePolicy GetSizePolice()
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        return sizePolicy;
    }

    TablePrivate::TablePrivate(const QString &iName, const QJsonDocument &iData, const QJsonDocument &iPersonalPermissions, const QJsonDocument &iPermissions, QWidget *parent) :
    QWidget(parent), _name(iName)
    {
        if (iData.isEmpty() || iPersonalPermissions.isEmpty() || iPermissions.isEmpty())
        {
            Q_ASSERT(false);
            return;
        }

        QSizePolicy sizePolicy = GetSizePolice();
        setSizePolicy(sizePolicy);

        QGridLayout *gridLayout = new QGridLayout(this);

        QSplitter *splitter = new QSplitter(this);
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Vertical);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicySplitter = GetSizePolice();
        sizePolicySplitter.setVerticalStretch(20);
        splitter->setSizePolicy(sizePolicySplitter);

        QGroupBox *data = new QGroupBox(splitter);
        data->setObjectName("personalData");
        data->setTitle("Личные данные");
        data->setSizePolicy(sizePolicy);

        QWidget *verticalLayoutWidget = new QWidget(splitter);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");

        QGridLayout *dataLayout = new QGridLayout(data);
        dataLayout->setObjectName("dataLayout");

        QGridLayout *buttonLayout = new QGridLayout(verticalLayoutWidget);
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        QCheckBox *autoUpdate = new QCheckBox(verticalLayoutWidget);
        connect(autoUpdate, SIGNAL(clicked(bool)), parent, SLOT(onAutoUpdateClicked(bool)));
        autoUpdate->setObjectName("autoUpdate");
        autoUpdate->setText("Автоматическое обновление");
        autoUpdate->setSizePolicy(sizePolicy);
        autoUpdate->setChecked(true);

        QPushButton *update= new QPushButton(verticalLayoutWidget);
        connect(update, SIGNAL(clicked()), parent, SLOT(onUpdateClicked()));
        update->setObjectName("update");
        update->setText("Обновить");
        update->setSizePolicy(sizePolicy);

        QPushButton *revert = new QPushButton(verticalLayoutWidget);
        connect(revert, SIGNAL(clicked()), parent, SLOT(onRevertClicked()));
        revert->setObjectName("revert");
        revert->setText("Откатить");
        revert->setSizePolicy(sizePolicy);

        QPushButton *exit = new QPushButton(verticalLayoutWidget);
        connect(exit, SIGNAL(clicked()), parent, SLOT(onExitClicked()));
        exit->setObjectName("exit");
        exit->setText("Выход");
        exit->setEnabled(true);
        exit->setSizePolicy(sizePolicy);

        splitter->addWidget(data);
        splitter->addWidget(verticalLayoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);
        setLayout(gridLayout);

        setEditStrategy(autoUpdate->isChecked() ? TablePrivate::EditStrategy::OnFieldChange : TablePrivate::EditStrategy::OnManualSubmit);

        const auto fieldNames = Client::Employee::getFieldNames();
        for (decltype(fieldNames.size()) i = 0, I = fieldNames.size(); i < I; ++i)
        {
            const auto& [field, name] = fieldNames[i];
            const QJsonObject object_data = iData.object(); // обязательно нужно определить
            const QJsonObject object_permissions = iPersonalPermissions.object(); // обязательно нужно определить

            auto it_data = object_data.find(field);
            auto it_permissions = object_permissions.find(field);
            if (it_data != object_data.end() && it_permissions != object_permissions.end())
            {
                QLabel *label = new QLabel(this);
                label->setObjectName(field);
                label->setSizePolicy(sizePolicy);
                label->setText(name);
                dataLayout->addWidget(label, i, 0, 1, 1);

                if (field == Client::Employee::role() ||
                    field == Client::Employee::sex())
                {
                    QComboBox *comboBox = new QComboBox(this);
                    comboBox->setObjectName(field);
                    if (field == Client::Employee::role())
                        comboBox->addItems(Client::Employee::getRoles());
                    else if (field == Client::Employee::sex())
                        comboBox->addItems(Client::Employee::getSex());

                    if (it_data->isString())
                        comboBox->setCurrentText(it_data->toString());
                    comboBox->setSizePolicy(sizePolicy);
                    comboBox->setStyleSheet(QString::fromUtf8("QComboBox {\n    border: 1px solid gray;\n}\n\nQComboBox::drop-down {\n    border-color: transparent;\n}"));
                    dataLayout->addWidget(comboBox, i, 1, 1, 1);
                    _data.push_back({label, comboBox});
                }
                else if (field == Client::Employee::salary())
                {
                    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
                    spinBox->setBackgroundRole(QPalette::ColorRole::BrightText);
                    spinBox->setAccessibleDescription("Введите " + name.toLower());
                    spinBox->setRange(0, 1000000);
                    if (it_data->isDouble())
                        spinBox->setValue(it_data->toDouble());
                    spinBox->setSizePolicy(GetSizePolice());
                    spinBox->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {\n border: 1px solid gray;\n }"));
                    dataLayout->addWidget(spinBox, i, 1, 1, 1);
                    _data.push_back({label, spinBox});
                }
                else
                {
                    QLineEdit *lineEdit = new QLineEdit(this);
                    lineEdit->setObjectName(field);
                    lineEdit->setPlaceholderText("Введите " + name.toLower());

                    if (field == Client::Employee::passport() ||
                        field == Client::Employee::phone())
                    {
                        lineEdit->setValidator(new UInt64Validator(0, 9999999999, this) );
                    }
                    else if (field == Client::Employee::password())
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

                    lineEdit->setSizePolicy(sizePolicy);
                    lineEdit->setStyleSheet(QString::fromUtf8("QTextEdit {\n"
            "    border: 1px solid gray;\n"
            "}"));
                    dataLayout->addWidget(lineEdit, i, 1, 1, 1);
                    _data.push_back({label,lineEdit});
                }
            }
        }

        const QJsonObject subobject_permissions = iPermissions.object(); // обязательно нужно определить
        auto show_db = subobject_permissions.find("show_db");
        auto create_user = subobject_permissions.find("create_user");
        auto delete_user = subobject_permissions.find("delete_user");
        if (show_db != subobject_permissions.end() &&
            create_user != subobject_permissions.end() &&
            delete_user != subobject_permissions.end())
        {
            if (show_db->isBool())
            {
                QPushButton *showDatabase = new QPushButton("Показать базу данных", this);
                showDatabase->setSizePolicy(sizePolicy);
                connect(showDatabase, SIGNAL(clicked()), parent, SLOT(showDatabase()));
                buttonLayout->addWidget(showDatabase, 1, 1, 1, 1);
            }

            if (create_user->isBool())
            {
                QPushButton *createUser = new QPushButton("Создать пользователя", this);
                connect(createUser, SIGNAL(clicked()), parent, SLOT(onCreateUserClicked()));
                createUser->setObjectName("createUser");
                createUser->setSizePolicy(sizePolicy);
                createUser->setEnabled(false);
                buttonLayout->addWidget(createUser, 1, 2, 1, 1);
            }

            if (delete_user->isBool())
            {
                QStackedWidget* stackedWidget = new QStackedWidget(verticalLayoutWidget);
                stackedWidget->setObjectName("changeUser");
                stackedWidget->setSizePolicy(sizePolicy);

                QPushButton *deleteUser = new QPushButton("Удалить пользователя", this);
                connect(deleteUser, SIGNAL(clicked()), parent, SLOT(onDeleteUserClicked()));
                deleteUser->setObjectName("deleteUser");
                deleteUser->setSizePolicy(sizePolicy);
                deleteUser->setEnabled(false);
                stackedWidget->addWidget(deleteUser);

                QPushButton *restoreUser = new QPushButton("Восстановить пользователя", this);
                connect(restoreUser, SIGNAL(clicked()), parent, SLOT(onRestoreUserClicked()));
                restoreUser->setObjectName("restoreUser");
                restoreUser->setSizePolicy(sizePolicy);
                stackedWidget->addWidget(restoreUser);

                stackedWidget->setCurrentWidget(deleteUser);
                buttonLayout->addWidget(stackedWidget, 1, 3, 1, 1);
            }
        }

        buttonLayout->addWidget(revert, 1, 0, 1, 1);
        buttonLayout->addWidget(autoUpdate, 0, 0, 1, buttonLayout->columnCount() / 2);
        buttonLayout->addWidget(update, 0, 2, 1, buttonLayout->columnCount() / 2);
        buttonLayout->addWidget(exit, 2, 0, 1, buttonLayout->columnCount());
        buttonLayout->addWidget(new QProgressBar(qobject_cast<const Table*>(parent)->_requester->getProgressBar()), 3, 0, 1, buttonLayout->columnCount());

//        data->adjustSize();
//        data->update();
//        dataLayout->update();
//        buttonLayout->update();
//        splitter->update();
    }

    TablePrivate::TablePrivate(const QString &iName, QWidget *parent) : QWidget(parent), _name(iName)
    {
        setEditStrategy(TablePrivate::EditStrategy::OnManualSubmit);
        QSizePolicy sizePolicy = GetSizePolice();

        QGridLayout *gridLayout = new QGridLayout(this);

        QSplitter *splitter = new QSplitter(this);
        splitter->setOrientation(Qt::Vertical);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicySplitter = GetSizePolice();
        sizePolicySplitter.setVerticalStretch(20);
        splitter->setSizePolicy(sizePolicySplitter);

        QGroupBox *data = new QGroupBox(splitter);
        data->setObjectName("newEmployeeData");
        data->setTitle("Данные нового сотрудника");
        data->setSizePolicy(sizePolicy);

        QWidget *verticalLayoutWidget = new QWidget(splitter);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");

        QGridLayout *dataLayout = new QGridLayout(data);
        dataLayout->setObjectName("dataLayout");

        QGridLayout *buttonLayout = new QGridLayout(verticalLayoutWidget);
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        const auto fieldNames = Client::Employee::getFieldNames();
        for (decltype(fieldNames.size()) i = 1, I = fieldNames.size(); i < I; ++i)
        {
            const auto& [field, name] = fieldNames[i];

            QLabel *label = new QLabel(this);
            label->setObjectName(field);
            label->setSizePolicy(GetSizePolice());
            label->setText(name);
            dataLayout->addWidget(label, i, 0, 1, 1);

            if (field == Client::Employee::role() ||
                field == Client::Employee::sex())
            {
                QComboBox *comboBox = new QComboBox(this);
                comboBox->setObjectName(field);
                comboBox->setPlaceholderText("Введите " + name.toLower());
                if (field == Client::Employee::role())
                    comboBox->addItems(Client::Employee::getRoles());
                else if (field == Client::Employee::sex())
                    comboBox->addItems(Client::Employee::getSex());
                comboBox->setSizePolicy(GetSizePolice());
                comboBox->setStyleSheet(QString::fromUtf8("QComboBox {\n    border: 1px solid gray;\n}\n\nQComboBox::drop-down {\n    border-color: transparent;\n}"));
                dataLayout->addWidget(comboBox, i, 1, 1, 1);
                _data.push_back({label, comboBox});
            }
            else if (field == Client::Employee::salary())
            {
                QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
                spinBox->setBackgroundRole(QPalette::ColorRole::BrightText);
                spinBox->setObjectName(field);
                spinBox->setAccessibleDescription("Введите " + name.toLower());
                spinBox->setRange(0, 1000000);
                spinBox->setValue(10000);
                spinBox->setSizePolicy(GetSizePolice());
                spinBox->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {\n border: 1px solid gray;\n }"));
                dataLayout->addWidget(spinBox, i, 1, 1, 1);
                _data.push_back({label, spinBox});
            }
            else
            {
                QLineEdit *lineEdit = new QLineEdit(this);
                if (field == Client::Employee::passport() ||
                    field == Client::Employee::phone())
                {
                    lineEdit->setValidator(new UInt64Validator(0, 9999999999, this) );
                }
                lineEdit->setObjectName(field);
                lineEdit->setPlaceholderText("Введите " + name.toLower());
                lineEdit->setSizePolicy(GetSizePolice());
                lineEdit->setStyleSheet(QString::fromUtf8("QTextEdit {\n"
        "    border: 1px solid gray;\n"
        "}"));
                dataLayout->addWidget(lineEdit, i, 1, 1, 1);
                _data.push_back({label, lineEdit});
            }
        }

        QPushButton *cancel= new QPushButton(verticalLayoutWidget);
        connect(cancel, SIGNAL(clicked()), parent, SLOT(onCancelClicked()));
        cancel->setObjectName("cancel");
        cancel->setText("Отмена");
        cancel->setSizePolicy(sizePolicy);

        QPushButton *addUser = new QPushButton(verticalLayoutWidget);
        connect(addUser, SIGNAL(clicked()), parent, SLOT(onAddUserClicked()));
        addUser->setObjectName("addUser");
        addUser->setText("Добавить в базу данных");
        addUser->setSizePolicy(sizePolicy);

        buttonLayout->addWidget(cancel, 0, 0, 1, 1);
        buttonLayout->addWidget(addUser, 0, 1, 1, 1);

        splitter->addWidget(data);
        splitter->addWidget(verticalLayoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);
        setLayout(gridLayout);
    }

    TablePrivate::~TablePrivate() {}

    void TablePrivate::setEditStrategy(EditStrategy iStrategy)
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

    void TablePrivate::sendUserData(const QString &iFieldName, const std::function<void(QWidget*)>& handleLineEdit)
    {
        if (!handleLineEdit)
            return;

        for (const auto &[label, lineEdit] : _data)
        {
            if (lineEdit->objectName() == iFieldName)
            {
                handleLineEdit(lineEdit);
                return;
            }
        }
    }

    void TablePrivate::update()
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
        auto found = std::find_if(std::begin(_data), std::end(_data), [&lineEdit](const auto& data) { return data.second == lineEdit; });
        if (found == std::end(_data))
        {
            Q_ASSERT(false);
            return;
        }

        const auto& [label, widget] = *found;
        const QString& columnRus = label->text();
        QJsonObject record;
        auto columnEng = std::find_if(std::begin(Client::Employee::getFieldNames()), std::end(Client::Employee::getFieldNames()), [&columnRus](const auto& field)
            {
                return field.second == columnRus;
            })->first;

        QString value;
        if (auto lineEdit = qobject_cast<const QLineEdit*>(widget))
        {
            value = lineEdit->text();
        }
        else if (auto comboBox = qobject_cast<const QComboBox*>(widget))
        {
            value = comboBox->currentText();
        }

        record.insert("column", columnEng);
        record.insert("value", value);

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
                            if (object.value("value") != value)
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

    void TablePrivate::submitAll()
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
}
