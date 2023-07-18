#include "tableView.h"
#include "client.h"
#include "delegate.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonTableModel>
#include <QMessageBox>
#include <QTimer>

#include <QHeaderView>


namespace Client
{
    TableView::TableView( QWidget *parent) : QTableView(parent)
    {
        setObjectName(QString::fromUtf8("tableView"));
        setItemDelegate(new Delegate(this));
        setSortingEnabled(true);
        setMouseTracking(true);
        setMinimumHeight(700);
        setMinimumWidth(800);
//        setMinimumSize(QSize(803, 654));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(10);
        setSizePolicy(sizePolicy);
        setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setEditTriggers(QAbstractItemView::DoubleClicked);
        horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        horizontalHeader()->setCascadingSectionResizes(false);
        horizontalHeader()->setHighlightSections(false);
    }

    void TableView::setEditStrategy(EditStrategy iStrategy) noexcept
    {
        if (_model)
            _model->setEditStrategy(static_cast<QJsonTableModel::EditStrategy>(iStrategy));
    }

    void TableView::setModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions)
    {
        setModel(_model = new QJsonTableModel(iName, QJsonDocument(iDatabase), QJsonDocument(iPermissions), this));
        connect(_model, SIGNAL(sendCreateRequest(const QByteArray&, const HandleResponse&)),
                this, SIGNAL(sendCreateData(const QByteArray&, const HandleResponse&)));
        connect(_model, SIGNAL(sendDeleteRequest(const QByteArray&, const HandleResponse&)),
                this, SIGNAL(sendDeleteData(const QByteArray&, const HandleResponse&)));
        connect(_model, SIGNAL(sendUpdateRequest(const QByteArray&, const HandleResponse&)),
                this, SIGNAL(sendUpdateData(const QByteArray&, const HandleResponse&)));

    }

    void TableView::setModel(const QString& iName, const QJsonDocument &iDatabase)
    {
        setModel(_model = new QJsonTableModel(iName, QJsonDocument(iDatabase), this));
    }

    const QAbstractItemModel *TableView::getModel() const noexcept
    {
        return _model;
    }

    void TableView::setModel(QAbstractItemModel *model)
    {
        QTableView::setModel(_model = qobject_cast<QJsonTableModel*>(model));
        horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(10, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(12, QHeaderView::Interactive);
        sortByColumn(0, Qt::SortOrder::AscendingOrder); // Может падать
    }

    void TableView::submitAll()
    {
        if (_model)
            _model->submitAll();
    }

    bool TableView::addUser()
    {
        if (!_model)
            return false;

        QJsonObject record;
        record.insert(Client::Employee::id(), _model->rowsCount());

        const auto fieldNames = Client::Employee::getFieldNames();

        for (int i = 1, I = fieldNames.size(); i < I; ++i)
        {
            bool result = false;
            QPair<QString, QString> field = fieldNames[i];
            std::function<void(QWidget*)> handleLineEdit = [&](QWidget* widget)
            {
                if (widget)
                {
                    QString value;
                    if (const QLineEdit* lineEdit = qobject_cast<const QLineEdit*>(widget))
                    {
                        value = lineEdit->text();
                    }
                    else if (const QComboBox* comboBox = qobject_cast<const QComboBox*>(widget))
                    {
                        value = comboBox->currentText();
                    }
                    else if (const QDoubleSpinBox *spinBox = qobject_cast<const QDoubleSpinBox*>(widget))
                    {
                        value = spinBox->text();
                    }

                    if (_model->checkField(_model->rowsCount(), i, value))
                    {
                        record.insert(field.first, value);
                        QPalette palette = widget->palette();
                        palette.setColor(QPalette::PlaceholderText, Qt::white);
                        palette.setColor(QPalette::Text, Qt::black);
                        widget->setPalette(palette);
                        result = true;
                    }
                    else
                    {
                        QPalette palette = widget->palette();
                        palette.setColor(QPalette::PlaceholderText, Qt::red);
                        palette.setColor(QPalette::Text, Qt::red);
                        widget->setPalette(palette);
                    }
                }
            };

            emit getUserData(field.first, handleLineEdit);

            if (!result)
                return false;
        }

        _model->addRow(record);
        const QString message = "Сотрудник успешно добавлен!";
        QMessageBox information(QMessageBox::Icon::Information, tr("Сообщение"), message, QMessageBox::NoButton, this);
        QTimer::singleShot(1000, &information, &QMessageBox::close);
        information.exec();
        qDebug() << "Сообщение: " << message;
        return true;
    }

    bool TableView::deleteUser()
    {
        if (!_model)
            return false;

        if (QItemSelectionModel *select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto &selectedRow : select->selectedRows())
                {
                    _model->deleteRow(selectedRow.row());
                }
            }
        }

        return false;
    }

    void TableView::restoreUser()
    {
        if (!_model)
            return;

        if (QItemSelectionModel *select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto &selectedRow : select->selectedRows())
                {
                    _model->restoreRow(selectedRow.row());
                }
            }
        }
    }

    std::optional<bool> TableView::canDelete()
    {
        if (!_model)
            return {};

        std::optional<bool> allDeleted;

        if (QItemSelectionModel *select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto &selectedRow : select->selectedRows())
                {
                    bool canDelete = _model->canDeleteRow(selectedRow.row());
                    if (allDeleted.has_value() && (*allDeleted & canDelete))
                        return {};

                    allDeleted = canDelete;
                }
            }
        }

        return allDeleted;
    }

    void TableView::valueSearchChanged(const QString &iValue)
    {
        if (!_model)
            return;

        std::for_each(_hiddenIndices.begin(), _hiddenIndices.end(), [this](const int index)
        {
            setRowHidden(index, false);
        });

        _hiddenIndices = _model->valueSearch(iValue);
        std::for_each(_hiddenIndices.begin(), _hiddenIndices.end(), [this](const int index)
        {
            setRowHidden(index, true);
        });
    }

    void TableView::clearSearchChanged()
    {
        std::for_each(_hiddenIndices.begin(), _hiddenIndices.end(), [this](const int index)
        {
            setRowHidden(index, false);
        });

        _hiddenIndices.clear();
    }
}
