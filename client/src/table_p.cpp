#include "table_p.h"
#include "cache.h"
#include "client.h"
#include "session.h"
#include "utils.h"

#include <LineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QGridLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStringListModel>
#include <QTimer>
#include <Validator>

#include <ranges>


namespace Client
{
    static QString pushButtonStyle(":enabled {\n"
                                    "border-left:20px solid qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(32, 180, 214,150), stop:1 rgba(0, 0, 0, 0));\n"
                                    "border-right:20px solid qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:0, stop:0 rgba(32, 180, 214,150), stop:1 rgba(0, 0, 0, 0));\n"
                                    "border-top:10px solid qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(32, 180, 214,150), stop:1 rgba(0, 0, 0, 0));\n"
                                    "border-bottom:10px solid qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(32, 180, 214,150), stop:1 rgba(0, 0, 0, 0));\n"
                                    "color:rgba(255, 255, 255, 210);\n"
                                    "}\n"
                                    ":disabled { \n"
                                    "background-color:rgba(255, 255, 255, 0); "
                                    "color:rgba(255, 255, 255, 100);"
                                    "}\n"
                                    ":hover{\n"
                                    "background-color: qlineargradient(spread:pad, x1:0, y1:0.505682, x2:1, y2:0.477, stop:0 rgba(40, 67, 98, 219), stop:1 rgba(105, 118, 132, 226));\n"
                                    "}\n"
                                    ":pressed{\n"
                                    "padding-left:5px;\n"
                                    "padding-top:5px;\n"
                                    "background-color:rgba(105, 118, 132, 200);\n"
                                    "}");

    static QPalette palette;

    static consteval const QSizePolicy GetSizePolice() noexcept
    {
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        return sizePolicy;
    }

    static const QPalette GetPaletteLight() noexcept
    {
        palette.setColor(QPalette::PlaceholderText, Qt::lightGray);
        palette.setColor(QPalette::Text, Qt::black);
        return palette;
    }

    static const QPalette GetPaletteGreen() noexcept
    {
        palette.setColor(QPalette::PlaceholderText, Qt::green);
        palette.setColor(QPalette::Text, Qt::green);
        return palette;
    }

    static const QPalette GetPaletteRed() noexcept
    {
        palette.setColor(QPalette::PlaceholderText, Qt::red);
        palette.setColor(QPalette::Text, Qt::red);
        return palette;
    }

    TablePrivate::TablePrivate(const QString& iName,
                               const QJsonDocument& iData,
                               const QJsonDocument& iPersonalPermissions,
                               const QJsonDocument& iPermissions,
                               QWidget* parent) :
        QWidget(parent),
        _isRoleChanged(false),
        _name(iName)
    {
        if (iData.isEmpty() || iPersonalPermissions.isEmpty() || iPermissions.isEmpty())
        {
            Q_ASSERT(false);
            return;
        }

        setObjectName(QString::fromUtf8("personalData"));
        QSizePolicy sizePolicy = GetSizePolice();
        setSizePolicy(sizePolicy);
        setMinimumHeight(700);

        QSplitter* splitter = new QSplitter(this);
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Vertical);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicySplitter = GetSizePolice();
        sizePolicySplitter.setVerticalStretch(20);
        sizePolicySplitter.setHorizontalStretch(20);
        splitter->setSizePolicy(sizePolicySplitter);

        QGroupBox* data = new QGroupBox(splitter);
        data->setObjectName("personalData");
        data->setTitle("Личные данные");
        data->setSizePolicy(sizePolicy);

        QWidget* verticalLayoutWidget = new QWidget(splitter);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setSizePolicy(sizePolicy);

        QGridLayout* dataLayout = new QGridLayout(data);
        dataLayout->setObjectName("dataLayout");

        QGridLayout* buttonLayout = new QGridLayout(verticalLayoutWidget);
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        QFont fontLabel;
        fontLabel.setUnderline(true);

        QSizePolicy sizePolicyLine = GetSizePolice();
        sizePolicyLine.setHorizontalStretch(1);

        const QJsonObject object_data = iData.object(); // обязательно нужно определить
        const QJsonObject object_permissions = iPersonalPermissions.object(); // обязательно нужно определить

        const auto fieldNames = Client::Employee::getFieldNames();
        for (const auto i : std::views::iota(0, fieldNames.size()))
        {
            const auto& [field, name] = fieldNames[i];
            const QString toolTip = Client::Employee::helpFields()[field];
            QString placeholderText = toolTip;

            auto it_data = object_data.find(field);
            auto it_permissions = object_permissions.find(field);
            if (it_data != object_data.end())
            {
                QLabel* label = new QLabel(name, this);
                label->setFont(fontLabel);
                label->setSizePolicy(sizePolicy);
                dataLayout->addWidget(label, i, 0, 1, 1);

                QWidget* widget = nullptr;

                if (field == Client::Employee::role() ||
                    field == Client::Employee::sex())
                {
                    QComboBox *comboBox = new QComboBox(this);
                    connect(comboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(update(const QString&)));
                    comboBox->setObjectName(field);
                    comboBox->setToolTip(toolTip);
                    comboBox->setPlaceholderText(placeholderText);
                    if (field == Client::Employee::role())
                        comboBox->addItems(Client::Employee::getRoles());
                    else if (field == Client::Employee::sex())
                        comboBox->addItems(Client::Employee::getSex());
                    if (it_data->isString())
                    {
                        comboBox->blockSignals(true);
                        comboBox->setCurrentText(it_data->toString());
                        comboBox->blockSignals(false);
                    }
                    else
                        Q_ASSERT(false);
                    comboBox->setSizePolicy(sizePolicy);
                    comboBox->setStyleSheet(QString::fromUtf8("QComboBox {border: 1px solid gray; padding: 0px;} QComboBox::drop-down {border-color: transparent;}"));
                    widget = comboBox;

                    _dataCache.emplace(field, comboBox->currentText());
                }
                else if (field == Client::Employee::salary())
                {
                    QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
                    connect(spinBox, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                    spinBox->setBackgroundRole(QPalette::ColorRole::BrightText);
                    spinBox->setObjectName(field);
                    spinBox->setToolTip(toolTip);
                    spinBox->setAccessibleDescription(placeholderText);
                    spinBox->setRange(0, 1000000);
                    spinBox->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                    if (it_data->isDouble())
                    {
                        spinBox->blockSignals(true);
                        spinBox->setValue(it_data->toDouble());
                        spinBox->blockSignals(false);
                    }
                    else
                        Q_ASSERT(false);
                    spinBox->setSizePolicy(GetSizePolice());
                    spinBox->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {border: 1px solid gray;} QDoubleSpinBox:focus {border: 4px solid #a5cdff;}"));
                    widget = spinBox;

                    _dataCache.emplace(field, spinBox->text());
                }
                else
                {
                    QLineEdit* lineEdit = nullptr;

                    if (field == Client::Employee::id())
                    {
                        lineEdit = new QLineEdit(this);
                        lineEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), this));
                        lineEdit->setEnabled(false);
                    }
                    else if (field == Client::Employee::name() ||
                             field == Client::Employee::surname() ||
                             field == Client::Employee::patronymic())
                    {
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new TextValidator(parent));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::dateOfBirth() ||
                             field == Client::Employee::dateOfHiring())
                    {
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, this));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::passport())
                    {
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, this));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::phone())
                    {
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, this));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::workingHours())
                    {
                        placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new TextValidator(parent));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::password())
                    {
                        placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setEchoMode(QLineEdit::Password);
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else if (field == Client::Employee::email())
                    {
                        lineEdit = new LineEdit(false, this);
                        connect(lineEdit, SIGNAL(startingFocus()), this, SLOT(createEmail()));
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setClearButtonEnabled(true);
                    }
                    else
                    {
                        lineEdit = new QLineEdit(this);
                        connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update(const QString&)));
                        lineEdit->setValidator(new TextValidator(parent));
                        lineEdit->setClearButtonEnabled(true);
                    }

                    lineEdit->blockSignals(true);
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

                    lineEdit->blockSignals(false);
                    lineEdit->setObjectName(field);
                    lineEdit->setToolTip(toolTip);
                    lineEdit->setPlaceholderText(placeholderText);
                    lineEdit->setSizePolicy(sizePolicyLine);
                    lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {border: 1px solid gray;} QLineEdit:focus {border: 4px solid #a5cdff;}"));
                    lineEdit->setAttribute(Qt::WA_MacShowFocusRect, true);
                    widget = lineEdit;
                    widget->clearFocus();

                    _dataCache.emplace(field, lineEdit->text());
                }

                if (it_permissions != object_permissions.end() && it_permissions->isString())
                {
                    widget->setEnabled(it_permissions->toString() == "write");
                }

                dataLayout->addWidget(widget, i, 1, 1, 1);
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
            if (create_user->isBool() && create_user->toBool() == true)
            {
                QPushButton* createUser = new QPushButton("Создать пользователя", verticalLayoutWidget);
                connect(createUser, SIGNAL(clicked()), parent, SLOT(onCreateUserClicked()));
                createUser->setIcon(QPixmap(QString::fromUtf8("../images/add.png")));
                createUser->setObjectName("createUser");
                createUser->setToolTip("Создать пользователя");
                createUser->setSizePolicy(sizePolicy);
                createUser->setEnabled(false);
                createUser->setStyleSheet(pushButtonStyle);
                buttonLayout->addWidget(createUser, 1, 1, 1, 1);
            }

            if (delete_user->isBool() && delete_user->toBool() == true)
            {
                QPushButton* deleteUser = new QPushButton("Удалить пользователя", verticalLayoutWidget);
                connect(deleteUser, SIGNAL(clicked()), parent, SLOT(onDeleteUserClicked()));
                deleteUser->setIcon(QPixmap(QString::fromUtf8("../images/delete.png")));
                deleteUser->setObjectName("deleteUser");
                deleteUser->setToolTip("Удалить пользователя из базы данных");
                deleteUser->setSizePolicy(sizePolicy);
                deleteUser->setEnabled(false);
                deleteUser->setStyleSheet(pushButtonStyle);
                buttonLayout->addWidget(deleteUser, 1, 2, 1, 1);

                QPushButton* restoreUser = new QPushButton("Восстановить пользователя", verticalLayoutWidget);
                connect(restoreUser, SIGNAL(clicked()), parent, SLOT(onRestoreUserClicked()));
                restoreUser->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
                restoreUser->setObjectName("restoreUser");
                restoreUser->setToolTip("Восстановить пользователя в базе данных");
                restoreUser->setSizePolicy(sizePolicy);
                restoreUser->setEnabled(true);
                restoreUser->setVisible(false);
                restoreUser->setStyleSheet(pushButtonStyle);
                buttonLayout->addWidget(restoreUser, 1, 2, 1, 1);
            }

            if (show_db->isBool() && show_db->toBool() == true)
            {
                QPushButton* showDatabase = new QPushButton("Показать базу данных", verticalLayoutWidget);
                connect(showDatabase, SIGNAL(clicked()), parent, SLOT(showDatabase()));
                showDatabase->setIcon(QPixmap(QString::fromUtf8("../images/show.png")));
                showDatabase->setObjectName("showDatabase");
                showDatabase->setToolTip("Показать базу данных сотрудников");
                showDatabase->setSizePolicy(sizePolicy);
                showDatabase->setStyleSheet(pushButtonStyle);
                buttonLayout->addWidget(showDatabase, 1, 0, 1, 1);

                QPushButton* search = new QPushButton("Поиск", verticalLayoutWidget);
                connect(search, SIGNAL(clicked()), SLOT(onSearchClicked()));
                search->setObjectName("search");
                search->setToolTip("Поиск сотрудника в базе данных");
                search->setSizePolicy(sizePolicy);
                search->setEnabled(false);
                search->setStyleSheet(pushButtonStyle);
                buttonLayout->addWidget(search, 2, 0, 1, 1);

                QLineEdit* valueSearch = new QLineEdit(verticalLayoutWidget);
                valueSearch->setClearButtonEnabled(true);
                QList<QAction*> actionList = valueSearch->findChildren<QAction*>();
                if (!actionList.isEmpty())
                {
                    connect(actionList.first(), &QAction::triggered, this, [this]()
                    {
                        emit sendClearSearch();
                    });
                }
                connect(valueSearch, SIGNAL(returnPressed()), this, SLOT(onSearchClicked()));

                QCompleter* completer = new QCompleter(Session::getSession().Cache().getSearchWords(), valueSearch);
                completer->setCaseSensitivity(Qt::CaseInsensitive);

                valueSearch->setCompleter(completer);
                valueSearch->setObjectName("valueSearch");
                valueSearch->setToolTip("Для поиска сотрудника введите слово или часть слова");
                valueSearch->setPlaceholderText("Введите слово или часть слова");
                valueSearch->setSizePolicy(sizePolicy);

                const QIcon searchIcon("../images/search.png");
                valueSearch->addAction(std::move(searchIcon), QLineEdit::LeadingPosition);
                valueSearch->setStyleSheet(QString::fromUtf8("QLineEdit {border: 1px solid gray;} QLineEdit:focus {border: 4px solid #a5cdff;}"));
                valueSearch->setEnabled(false);
                buttonLayout->addWidget(valueSearch, 2, 1, 1, buttonLayout->columnCount() - 1);
            }
        }

        QCheckBox* autoUpdate = new QCheckBox("Автоматическое обновление", verticalLayoutWidget);
        connect(autoUpdate, SIGNAL(clicked(bool)), parent, SLOT(onAutoUpdateClicked(bool)));
        autoUpdate->setObjectName("autoUpdate");
        autoUpdate->setToolTip("Автоматически отправлять данные на сервер");
        autoUpdate->setSizePolicy(sizePolicy);
        buttonLayout->addWidget(autoUpdate, 0, 0, 1, 1);

        QPushButton* update = new QPushButton("Обновить", verticalLayoutWidget);
        connect(update, SIGNAL(clicked()), parent, SLOT(onUpdateClicked()));
        update->setIcon(QPixmap(QString::fromUtf8("../images/reload.png")));
        update->setToolTip("Отправить данные на сервер");
        update->setObjectName("update");
        update->setSizePolicy(sizePolicy);
        update->setStyleSheet(pushButtonStyle);
        buttonLayout->addWidget(update, 0, 1, 1, 1);

        QPushButton* revert = new QPushButton(verticalLayoutWidget);
        connect(revert, SIGNAL(clicked()), parent, SLOT(onRevertClicked()));
        revert->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
        revert->setObjectName("revert");
        revert->setToolTip("Вернуть предыдущие изменения");
        revert->setText("Откатить");
        revert->setSizePolicy(sizePolicy);
        revert->setStyleSheet(pushButtonStyle);
        buttonLayout->addWidget(revert, 0, 2, 1, buttonLayout->columnCount() / 2);

        QPushButton* exit = new QPushButton("Выход", verticalLayoutWidget);
        connect(exit, SIGNAL(clicked()), parent, SLOT(onExitClicked()));
        exit->setIcon(QPixmap(QString::fromUtf8("../images/exit.png")));
        exit->setObjectName("exit");
        exit->setToolTip("Вернуться в главное меню");
        exit->setEnabled(true);
        exit->setSizePolicy(sizePolicy);
        exit->setStyleSheet(pushButtonStyle);
        buttonLayout->addWidget(exit, buttonLayout->rowCount(), 0, 1, buttonLayout->columnCount());

        splitter->addWidget(data);
        splitter->addWidget(verticalLayoutWidget);

        QGridLayout* gridLayout = new QGridLayout(this);
        gridLayout->addWidget(splitter, 0, 0, 1, 1);
        setLayout(gridLayout);

        setEditStrategy(autoUpdate->isChecked() ? TablePrivate::EditStrategy::OnFieldChange : TablePrivate::EditStrategy::OnManualSubmit);
        adjustSize();
    }

    TablePrivate::TablePrivate(const QString& iName, QWidget* parent) :
        QWidget(parent),
        _isRoleChanged(false),
        _name(iName)
    {
        setObjectName(QString::fromUtf8("userData"));
        QSizePolicy sizePolicy = GetSizePolice();

        QGridLayout* gridLayout = new QGridLayout(this);

        QSplitter* splitter = new QSplitter(this);
        splitter->setOrientation(Qt::Vertical);
        splitter->setObjectName("splitter");
        QSizePolicy sizePolicySplitter = GetSizePolice();
        sizePolicySplitter.setVerticalStretch(20);
        splitter->setSizePolicy(sizePolicySplitter);

        QGroupBox* data = new QGroupBox(splitter);
        data->setObjectName("newEmployeeData");
        data->setTitle("Данные нового сотрудника");
        data->setSizePolicy(sizePolicy);

        QWidget* verticalLayoutWidget = new QWidget(splitter);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");

        QGridLayout* dataLayout = new QGridLayout(data);
        dataLayout->setObjectName("dataLayout");

        QGridLayout* buttonLayout = new QGridLayout(verticalLayoutWidget);
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        QFont fontLabel;
        fontLabel.setUnderline(true);

        QSizePolicy sizePolicyLine = GetSizePolice();
        sizePolicyLine.setHorizontalStretch(1);

        const auto fieldNames = Client::Employee::getFieldNames();
        for (const auto i : std::views::iota(1, fieldNames.size()))
        {
            const auto& [field, name] = fieldNames[i];
            const QString toolTip = Client::Employee::helpFields()[field];
            QString placeholderText = toolTip;

            QLabel* label = new QLabel(this);
            label->setFont(fontLabel);
            label->setSizePolicy(GetSizePolice());
            label->setText(name);
            dataLayout->addWidget(label, i, 0, 1, 1);

            if (field == Client::Employee::role() ||
                field == Client::Employee::sex())
            {
                QComboBox* comboBox = new QComboBox(this);
                comboBox->setObjectName(field);
                comboBox->setToolTip(toolTip);
                comboBox->setPlaceholderText(placeholderText);
                if (field == Client::Employee::role())
                    comboBox->addItems(Client::Employee::getRoles());
                else if (field == Client::Employee::sex())
                    comboBox->addItems(Client::Employee::getSex());
                comboBox->setSizePolicy(GetSizePolice());
                comboBox->setStyleSheet(QString::fromUtf8("QComboBox {border: 1px solid gray; padding: 0px;} QComboBox::drop-down {border-color: transparent;}"));
                dataLayout->addWidget(comboBox, i, 1, 1, 1);

                _dataCache.emplace(field, comboBox->currentText());
            }
            else if (field == Client::Employee::salary())
            {
                QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
                spinBox->setBackgroundRole(QPalette::ColorRole::BrightText);
                spinBox->setObjectName(field);
                spinBox->setToolTip(toolTip);
                spinBox->setAccessibleDescription(toolTip);
                spinBox->setRange(0, 1000000);
                spinBox->setValue(10000);
                spinBox->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                spinBox->setSizePolicy(GetSizePolice());
                spinBox->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {border: 1px solid gray;} QDoubleSpinBox:focus {border: 4px solid #a5cdff;}"));
                dataLayout->addWidget(spinBox, i, 1, 1, 1);

                _dataCache.emplace(field, spinBox->text());
            }
            else
            {
                QLineEdit* lineEdit = nullptr;

                if (field == Client::Employee::name() ||
                    field == Client::Employee::surname() ||
                    field == Client::Employee::patronymic())
                {
                    lineEdit = new QLineEdit(this);
                    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(createEmail()));
                    lineEdit->setValidator(new TextValidator(parent));
                }
                else if (field == Client::Employee::dateOfBirth() ||
                         field == Client::Employee::dateOfHiring())
                {
                    lineEdit = new QLineEdit(this);
                    lineEdit->setValidator(new UInt64Validator(0, 99999999, UInt64Validator::Mode::Date, this) );
                }
                else if (field == Client::Employee::passport())
                {
                    lineEdit = new QLineEdit(this);
                    lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Passport, this) );
                }
                else if (field == Client::Employee::phone())
                {
                    lineEdit = new QLineEdit(this);
                    lineEdit->setValidator(new UInt64Validator(0, 9999999999, UInt64Validator::Mode::Phone, this) );
                }
                else if (field == Client::Employee::workingHours())
                {
                    placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                    lineEdit = new QLineEdit(this);
                    lineEdit->setValidator(new TextValidator(parent));
                }
                else if (field == Client::Employee::password())
                {
                    placeholderText = placeholderText.left(placeholderText.indexOf("\n"));
                    lineEdit = new QLineEdit(this);
                    lineEdit->setEchoMode(QLineEdit::Password);
                }
                else if (field == Client::Employee::email())
                {
                    lineEdit = new LineEdit(false, this);
                    connect(lineEdit, SIGNAL(startingFocus()), this, SLOT(createEmail()));
                }
                else
                {
                    lineEdit = new QLineEdit(this);
                    lineEdit->setValidator(new TextValidator(parent));
                }

                lineEdit->setObjectName(field);
                lineEdit->setToolTip(toolTip);
                lineEdit->setPlaceholderText(placeholderText);
                lineEdit->setSizePolicy(sizePolicyLine);
                lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {border: 1px solid gray;} QLineEdit:focus {border: 4px solid #a5cdff;}"));
                lineEdit->setClearButtonEnabled(true);
                dataLayout->addWidget(lineEdit, i, 1, 1, 1);

                _dataCache.emplace(field, lineEdit->text());
            }
        }

        QPushButton* cancel = new QPushButton("Отмена", verticalLayoutWidget);
        connect(cancel, SIGNAL(clicked()), parent, SLOT(onCancelClicked()));
        cancel->setIcon(QPixmap(QString::fromUtf8("../images/cancel.png")));
        cancel->setObjectName("cancel");
        cancel->setToolTip("Отменить ввод данных нового пользователя");
        cancel->setSizePolicy(sizePolicy);
        cancel->setStyleSheet(pushButtonStyle);

        QPushButton* resetData = new QPushButton("Сбросить данные", verticalLayoutWidget);
        connect(resetData, SIGNAL(clicked()), SLOT(onResetDataClicked()));
        resetData->setIcon(QPixmap(QString::fromUtf8("../images/delete.png")));
        resetData->setObjectName("resetData");
        resetData->setToolTip("Сбросить данные нового пользователя");
        resetData->setSizePolicy(sizePolicy);
        resetData->setStyleSheet(pushButtonStyle);

        QPushButton* addUser = new QPushButton("Добавить в базу данных", verticalLayoutWidget);
        connect(addUser, SIGNAL(clicked()), parent, SLOT(onAddUserClicked()));
        addUser->setIcon(QPixmap(QString::fromUtf8("../images/add.png")));
        addUser->setObjectName("addUser");
        addUser->setToolTip("Добавить в базу данных нового пользователя");
        addUser->setSizePolicy(sizePolicy);
        addUser->setStyleSheet(pushButtonStyle);

        buttonLayout->addWidget(cancel, 0, 0, 1, 1);
        buttonLayout->addWidget(resetData, 0, 1, 1, 1);
        buttonLayout->addWidget(addUser, 0, 2, 1, 1);

        splitter->addWidget(data);
        splitter->addWidget(verticalLayoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);
        setLayout(gridLayout);
    }

    TablePrivate::~TablePrivate()
    {
        if (QLineEdit* value = findChild<QLineEdit*>("valueSearch"))
        {
            if (QStringListModel* model = qobject_cast<QStringListModel*>(value->completer()->model()))
            {
                Session::getSession().Cache().addSearchWords(model->stringList());
            }
        }
    }

    void TablePrivate::setEditStrategy(EditStrategy iStrategy)
    {
        if (iStrategy != _strategy)
        {
            _strategy = iStrategy;
            if (_strategy == OnFieldChange)
                submitAll();
        }
    }

    void TablePrivate::sendUserData(const QString& iFieldName, const HandleField& handleField)
    {
        if (!handleField)
            return;

        const auto fieldNames = Client::Employee::getFieldNames();
        for (const auto i : std::views::iota(1, fieldNames.size()))
        {
            if (fieldNames[i].first == iFieldName)
            {
                auto widget = findChild<QWidget*>(fieldNames[i].first);

                QString value;
                if (auto lineEdit = qobject_cast<QLineEdit*>(widget))
                {
                    value = lineEdit->text();
                }
                else if (auto comboBox = qobject_cast<QComboBox*>(widget))
                {
                    value = comboBox->currentText();
                }
                else if (QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget))
                {
                    value = spinBox->text();
                }

                if (handleField(value))
                {
                    widget->setPalette(GetPaletteLight());
                }
                else
                {
                    widget->setPalette(GetPaletteRed());
                }

                break;
            }
        }
    }

    void TablePrivate::update(const QString& iValue)
    {
        const auto id = findChild<const QLineEdit*>(Client::Employee::id());
        if (!id || _dataCache.empty())
            return;

        auto widget = qobject_cast<QWidget*>(sender());
        const QString field = widget->objectName();
        const QString& value = _dataCache[field];

        if (iValue == value)
        {
            for (const auto i : std::views::iota(0, _recordsCache.size()))
            {
                if (_recordsCache.at(i).isObject())
                {
                    const QJsonObject object = _recordsCache.at(i).toObject();
                    if (object.contains("column"))
                    {
                        if (object.value("column") == field)
                        {
                            widget->setPalette(GetPaletteLight());
                            _recordsCache.removeAt(i);
                            break;
                        }
                    }
                }
            }

            return;
        }

        if (field == Client::Employee::role())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Изменение должности",
                                                                      "Вы хотите изменить свою должность (будет произведет выход из личного кабинета)?",
                                                                      QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                _isRoleChanged = true;
            }
            else
            {
                auto comboBox = qobject_cast<QComboBox*>(widget);
                if (comboBox)
                {
                    widget->blockSignals(true);
                    comboBox->setCurrentText(value);
                    widget->blockSignals(false);
                    return;
                }
                else
                    Q_ASSERT(comboBox);
            }
        }

        widget->setPalette(GetPaletteGreen());

        QJsonObject record;
        record.insert(Client::Employee::id(), id->text().toLongLong());
        record.insert("column", field);
        record.insert("value", iValue);

        bool found = false;
        for (const auto i : std::views::iota(0, _recordsCache.size()))
        {
            if (_recordsCache.at(i).isObject())
            {
                const QJsonObject object = _recordsCache.at(i).toObject();
                if (object.contains("column") && object.contains("value"))
                {
                    if (object.value("column") == field)
                    {
                        found = true;
                        if (object.value("value") != iValue)
                        {
                            _recordsCache.replace(i, std::move(record));
                            break;
                        }
                    }
                }
            }
        }

        if (!found)
        {
            if (_isRoleChanged)
                _recordsCache.push_front(std::move(record));
            else
                _recordsCache.push_back(std::move(record));
        }

        if (_strategy == OnFieldChange)
            submitAll();

        widget->clearFocus();
    }

    void TablePrivate::submitAll()
    {
        if (!_recordsCache.empty())
        {
            sendRequest(QJsonDocument(QJsonObject{{_name, _recordsCache}}).toJson(), [&](const bool iResult, const QVariant& error)
            {
                if (iResult)
                {
                    qInfo() << "Персональные данные успешно обновлены";

                    if (_isRoleChanged)
                    {
                        _isRoleChanged = false;
                        emit logout();
                    }
                    else
                    {
                        while (!_recordsCache.empty())
                        {
                            if (_recordsCache.last().isObject())
                            {
                                const QJsonObject objectCache = _recordsCache.last().toObject();
                                if (objectCache.contains("column") && objectCache.contains("value"))
                                {
                                    QString field = objectCache.value("column").toString();
                                    if (auto widget = findChild<QWidget*>(field))
                                    {
                                        widget->setPalette(GetPaletteLight());
                                    }

                                    _dataCache[field] = objectCache.value("value").toString();
                                    _recordsCache.pop_back();
                                }
                            }
                        }
                    }
                }
                else
                {
                    for (const auto i : std::views::iota(0, _recordsCache.size()))
                    {
                        if (_recordsCache.at(i).isObject())
                        {
                            const QJsonObject objectCache = _recordsCache.at(i).toObject();
                            if (objectCache.contains("column") && objectCache.contains("value"))
                            {
                                QString field = objectCache.value("column").toString();
                                QString& value = _dataCache[field];

                                if (auto widget = findChild<QWidget*>(field))
                                {
                                    widget->blockSignals(true);
                                    if (auto lineEdit = qobject_cast<QLineEdit*>(widget))
                                    {
                                        lineEdit->setText(value);
                                    }
                                    else if (auto comboBox = qobject_cast<QComboBox*>(widget))
                                    {
                                        comboBox->setCurrentText(value);
                                    }
                                    else if (QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget))
                                    {
                                        spinBox->setValue(spinBox->valueFromText(value));
                                    }
                                    widget->blockSignals(false);
                                }
                            }
                        }
                    }

                    qWarning() << "Ошибка: " << error;
                    QMessageBox warning(QMessageBox::Icon::Warning, tr("Ошибка"), error.toString(), QMessageBox::NoButton, this);
                    QTimer::singleShot(1500, &warning, &QMessageBox::close);
                    warning.exec();
                }
            });
        }
        else
            qInfo() << "Пустые персональные данные для обновления ";
    }

    void TablePrivate::revertAll()
    {
        while (!_recordsCache.empty())
        {
            if (_recordsCache.last().isObject())
            {
                const QJsonObject objectCache = _recordsCache.last().toObject();
                if (objectCache.contains("column") && objectCache.contains("value"))
                {
                    QString field = objectCache.value("column").toString();
                    QString& value = _dataCache[field];

                    if (auto widget = findChild<QWidget*>(field))
                    {
                        widget->blockSignals(true);
                        if (auto lineEdit = qobject_cast<QLineEdit*>(widget))
                        {
                            lineEdit->setText(value);
                        }
                        else if (auto comboBox = qobject_cast<QComboBox*>(widget))
                        {
                            comboBox->setCurrentText(value);
                        }
                        else if (QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget))
                        {
                            spinBox->setValue(spinBox->valueFromText(value));
                        }
                        widget->blockSignals(false);
                    }

                    _recordsCache.pop_back();
                }
            }
        }

        qInfo() << "Персональные данные откатились";
    }

    bool TablePrivate::checkChanges() const noexcept
    {
        return !_recordsCache.empty();
    }

    void TablePrivate::onResetDataClicked()
    {
        for (auto it = _dataCache.cbegin(), end = _dataCache.cend(); it != end; ++it)
        {
            if (auto widget = findChild<QWidget*>(it.key()))
            {
                if (auto lineEdit = qobject_cast<QLineEdit*>(widget))
                {
                    lineEdit->clear();
                }
                else if (auto comboBox = qobject_cast<QComboBox*>(widget))
                {
                    comboBox->setCurrentIndex(-1);
                }
                else if (QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget))
                {
                    spinBox->setValue(10000);
                }
            }
        }
    }

    void TablePrivate::onSearchClicked()
    {
        if (QLineEdit* value = findChild<QLineEdit*>("valueSearch"))
        {
            if (QStringListModel* model = qobject_cast<QStringListModel*>(value->completer()->model()))
            {
                if (QStringList list = model->stringList(); !list.contains(value->text()))
                {
                    list.append(value->text());
                    model->setStringList(std::move(list));
                }
            }

            emit sendValueSearch(value->text());
        }
    }

    void TablePrivate::createEmail()
    {
        qInfo() << "Автосоздание почты";
        const QLineEdit* surname = findChild<const QLineEdit*>(Client::Employee::surname());
        const QLineEdit* name = findChild<const QLineEdit*>(Client::Employee::name());
        const QLineEdit* patronymic = findChild<const QLineEdit*>(Client::Employee::patronymic());
        LineEdit* email = findChild<LineEdit*>(Client::Employee::email());

        if (!surname || !name || !patronymic || !email)
            return;

        const QString newEmail = Utils::CreateEmail({ surname->text(), name->text(), patronymic->text() });
        email->setText(std::move(newEmail));
    }
}
