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
    QSizePolicy GetSizePolice() noexcept
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
        setMinimumHeight(700);

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
        verticalLayoutWidget->setSizePolicy(sizePolicy);

        QGridLayout *dataLayout = new QGridLayout(data);
        dataLayout->setObjectName("dataLayout");

        QGridLayout *buttonLayout = new QGridLayout(verticalLayoutWidget);
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        QSizePolicy sizePolicyLine = GetSizePolice();
        sizePolicyLine.setHorizontalStretch(1);

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
                QLabel *label = new QLabel(name, this);
                label->setObjectName(field);
                label->setSizePolicy(sizePolicy);
                dataLayout->addWidget(label, i, 0, 1, 1);

                if (field == Client::Employee::role() ||
                    field == Client::Employee::sex())
                {
                    QComboBox *comboBox = new QComboBox(this);
                    comboBox->setObjectName(field);
                    comboBox->setToolTip(Client::Employee::helpFields()[field]);
                    comboBox->setPlaceholderText(Client::Employee::helpFields()[field]);
                    if (field == Client::Employee::role())
                        comboBox->addItems(Client::Employee::getRoles());
                    else if (field == Client::Employee::sex())
                        comboBox->addItems(Client::Employee::getSex());

                    if (it_data->isString())
                        comboBox->setCurrentText(it_data->toString());
                    comboBox->setSizePolicy(sizePolicy);
                    comboBox->setStyleSheet(QString::fromUtf8("QComboBox {\n border: 1px solid gray;\n}\n\n QComboBox::drop-down {\n border-color: transparent;\n}"));
                    dataLayout->addWidget(comboBox, i, 1, 1, 1);
                    _data.push_back({label, comboBox});
                }
                else if (field == Client::Employee::salary())
                {
                    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
                    spinBox->setBackgroundRole(QPalette::ColorRole::BrightText);
                    spinBox->setToolTip(Client::Employee::helpFields()[field]);
                    spinBox->setAccessibleDescription(Client::Employee::helpFields()[field]);
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

                    if (field == Client::Employee::dateOfBirth() ||
                        field == Client::Employee::dateOfHiring())
                    {
                        lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, this) );
                    }
                    else if (field == Client::Employee::passport())
                    {
                        lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, this) );
                    }
                    else if (field == Client::Employee::phone())
                    {
                        lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, this) );
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

                    lineEdit->setToolTip(Client::Employee::helpFields()[field]);
                    lineEdit->setPlaceholderText(Client::Employee::helpFields()[field]);
                    lineEdit->setSizePolicy(sizePolicyLine);
                    lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n border: 1px solid gray;\n}"));
                    lineEdit->setClearButtonEnabled(true);
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
            if (create_user->isBool())
            {
                QPushButton *createUser = new QPushButton("Создать пользователя", verticalLayoutWidget);
                connect(createUser, SIGNAL(clicked()), parent, SLOT(onCreateUserClicked()));
                createUser->setIcon(QPixmap(QString::fromUtf8("../images/add.png")));
                createUser->setObjectName("createUser");
                createUser->setSizePolicy(sizePolicy);
                createUser->setEnabled(false);
                buttonLayout->addWidget(createUser, 1, 2, 1, 1);
            }

            if (delete_user->isBool())
            {
                QPushButton *deleteUser = new QPushButton("Удалить пользователя", verticalLayoutWidget);
                connect(deleteUser, SIGNAL(clicked()), parent, SLOT(onDeleteUserClicked()));
                deleteUser->setIcon(QPixmap(QString::fromUtf8("../images/delete.png")));
                deleteUser->setObjectName("deleteUser");
                deleteUser->setSizePolicy(sizePolicy);
                deleteUser->setEnabled(false);
                buttonLayout->addWidget(deleteUser, 1, 3, 1, 1);

                QPushButton *restoreUser = new QPushButton("Восстановить пользователя", verticalLayoutWidget);
                connect(restoreUser, SIGNAL(clicked()), parent, SLOT(onRestoreUserClicked()));
                restoreUser->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
                restoreUser->setObjectName("restoreUser");
                restoreUser->setSizePolicy(sizePolicy);
                restoreUser->setEnabled(true);
                restoreUser->setVisible(false);
                buttonLayout->addWidget(restoreUser, 1, 3, 1, 1);
            }

            if (show_db->isBool())
            {
                QPushButton *showDatabase = new QPushButton("Показать базу данных", verticalLayoutWidget);
                connect(showDatabase, SIGNAL(clicked()), parent, SLOT(showDatabase()));
                showDatabase->setIcon(QPixmap(QString::fromUtf8("../images/show.png")));
                showDatabase->setObjectName("showDatabase");
                showDatabase->setSizePolicy(sizePolicy);
                buttonLayout->addWidget(showDatabase, 1, 1, 1, 1);

                QPushButton *search = new QPushButton("Поиск", verticalLayoutWidget);
                connect(search, SIGNAL(clicked()), SLOT(onSearchClicked()));
                search->setObjectName("search");
                search->setSizePolicy(sizePolicy);
                buttonLayout->addWidget(search, 2, 0, 1, 1);

                QLineEdit *valueSearch = new QLineEdit(verticalLayoutWidget);
                valueSearch->setClearButtonEnabled(true);
                QList<QAction*> actionList = valueSearch->findChildren<QAction*>();
                if (!actionList.isEmpty())
                {
                    connect(actionList.first(), &QAction::triggered, this, [this]()
                    {
                        emit sendClearSearch();
                    });
                }
//                connect(valueSearch, SIGNAL(mousePressEvent(QMouseEvent *)), this, SLOT(onClearSearchClicked(QMouseEvent *)));
                valueSearch->setObjectName("valueSearch");
                valueSearch->setToolTip("Введите слово или часть слова");
                valueSearch->setPlaceholderText("Введите слово или часть слова");
                valueSearch->setSizePolicy(sizePolicy);

                const QIcon searchIcon("../images/search.png");
                valueSearch->addAction(searchIcon, QLineEdit::LeadingPosition);
                valueSearch->setStyleSheet(QString::fromUtf8("QLineEdit {\n border: 1px solid gray;\n}"));
                buttonLayout->addWidget(valueSearch, 2, 1, 1, buttonLayout->columnCount() - 1);
            }
        }

        QCheckBox *autoUpdate = new QCheckBox("Автоматическое обновление", verticalLayoutWidget);
        connect(autoUpdate, SIGNAL(clicked(bool)), parent, SLOT(onAutoUpdateClicked(bool)));
        autoUpdate->setObjectName("autoUpdate");
        autoUpdate->setSizePolicy(sizePolicy);
        autoUpdate->setChecked(true);
        buttonLayout->addWidget(autoUpdate, 0, 0, 1, buttonLayout->columnCount() / 2);

        QPushButton *update = new QPushButton("Обновить", verticalLayoutWidget);
        connect(update, SIGNAL(clicked()), parent, SLOT(onUpdateClicked()));
        update->setIcon(QPixmap(QString::fromUtf8("../images/reload.png")));
        update->setObjectName("update");
        update->setSizePolicy(sizePolicy);
        buttonLayout->addWidget(update, 0, 2, 1, buttonLayout->columnCount() / 2);

        QPushButton *revert = new QPushButton(verticalLayoutWidget);
        connect(revert, SIGNAL(clicked()), parent, SLOT(onRevertClicked()));
        revert->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
        revert->setObjectName("revert");
        revert->setText("Откатить");
        revert->setSizePolicy(sizePolicy);
        buttonLayout->addWidget(revert, 1, 0, 1, 1);

        QPushButton *exit = new QPushButton("Выход", verticalLayoutWidget);
        connect(exit, SIGNAL(clicked()), parent, SLOT(onExitClicked()));
        exit->setIcon(QPixmap(QString::fromUtf8("../images/exit.png")));
        exit->setObjectName("exit");
        exit->setEnabled(true);
        exit->setSizePolicy(sizePolicy);
        buttonLayout->addWidget(exit, (buttonLayout->rowCount() == 3) ? 3 : 2, 0, 1, buttonLayout->columnCount());

        splitter->addWidget(data);
        splitter->addWidget(verticalLayoutWidget);

        QGridLayout *gridLayout = new QGridLayout(this);
        gridLayout->addWidget(splitter, 0, 0, 1, 1);
        setLayout(gridLayout);

        setEditStrategy(autoUpdate->isChecked() ? TablePrivate::EditStrategy::OnFieldChange : TablePrivate::EditStrategy::OnManualSubmit);

        buttonLayout->addWidget(new QProgressBar(qobject_cast<const Table*>(parent)->_requester->getProgressBar()), (buttonLayout->rowCount() == 4) ? 4 : 3, 0, 1, buttonLayout->columnCount());

        adjustSize();
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

        QSizePolicy sizePolicyLine = GetSizePolice();
        sizePolicyLine.setHorizontalStretch(1);

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
                comboBox->setToolTip(Client::Employee::helpFields()[field]);
                comboBox->setPlaceholderText(Client::Employee::helpFields()[field]);
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
                spinBox->setToolTip(Client::Employee::helpFields()[field]);
                spinBox->setAccessibleDescription(Client::Employee::helpFields()[field]);
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

                if (field == Client::Employee::dateOfBirth() ||
                    field == Client::Employee::dateOfHiring())
                {
                    lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, this) );
                }
                else if (field == Client::Employee::passport())
                {
                    lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, this) );
                }
                else if (field == Client::Employee::phone())
                {
                    lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, this) );
                }
                lineEdit->setObjectName(field);
                lineEdit->setToolTip(Client::Employee::helpFields()[field]);
                lineEdit->setPlaceholderText(Client::Employee::helpFields()[field]);
                lineEdit->setSizePolicy(sizePolicyLine);
                lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n border: 1px solid gray;\n}"));
                lineEdit->setClearButtonEnabled(true);
                dataLayout->addWidget(lineEdit, i, 1, 1, 1);
                _data.push_back({label, lineEdit});
            }
        }

        QPushButton *cancel = new QPushButton("Отмена", verticalLayoutWidget);
        connect(cancel, SIGNAL(clicked()), parent, SLOT(onCancelClicked()));
        cancel->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
        cancel->setObjectName("cancel");
        cancel->setSizePolicy(sizePolicy);

        QPushButton *resetData = new QPushButton("Сбросить данные", verticalLayoutWidget);
        connect(resetData, SIGNAL(clicked()), SLOT(onResetDataClicked()));
        resetData->setIcon(QPixmap(QString::fromUtf8("../images/delete.png")));
        resetData->setObjectName("resetData");
        resetData->setSizePolicy(sizePolicy);

        QPushButton *addUser = new QPushButton("Добавить в базу данных", verticalLayoutWidget);
        connect(addUser, SIGNAL(clicked()), parent, SLOT(onAddUserClicked()));
        addUser->setIcon(QPixmap(QString::fromUtf8("../images/add.png")));
        addUser->setObjectName("addUser");
        addUser->setSizePolicy(sizePolicy);

        buttonLayout->addWidget(cancel, 0, 0, 1, 1);
        buttonLayout->addWidget(resetData, 0, 1, 1, 1);
        buttonLayout->addWidget(addUser, 0, 2, 1, 1);

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
        if (!lineEdit)
            return;

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

    void TablePrivate::onResetDataClicked()
    {
        for (auto& [_, widget]: _data)
        {
            if (auto lineEdit = qobject_cast<QLineEdit*>(widget))
            {
                lineEdit->clear();
            }
            else if (auto comboBox = qobject_cast<QComboBox*>(widget))
            {
                comboBox->clear();
            }
            else if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(widget))
            {
                spinBox->setValue(10000);
            }
        }
    }

    void TablePrivate::onSearchClicked()
    {
        if (QLineEdit* value = findChild<QLineEdit*>("valueSearch"))
        {
            emit sendValueSearch(value->text());
        }
    }
}
