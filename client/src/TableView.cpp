#include "client.h"
#include "tableView.h"
#include "delegate.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLayout>
#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonTableModel>
#include <QObject>
#include <QResizeEvent>
#include <QAbstractItemModel>


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
        setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setEditTriggers(QAbstractItemView::DoubleClicked);
//        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//        horizontalHeader()->setCascadingSectionResizes(true);
//        horizontalHeader()->setHighlightSections(false);
    }

    void TableView::setEditStrategy(EditStrategy iStrategy) noexcept
    {
        if (_model)
            _model->setEditStrategy(static_cast<QJsonTableModel::EditStrategy>(iStrategy));
    }

    void TableView::setModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions)
    {
        _model = new QJsonTableModel(iName, QJsonDocument(iDatabase), QJsonDocument(iPermissions), this);
        connect(_model, &QJsonTableModel::sendUpdateRequest, createData);
        connect(_model, &QJsonTableModel::sendUpdateRequest, deleteData);
        connect(_model, &QJsonTableModel::sendUpdateRequest, updateData);
        setModel(_model);
    }

    void TableView::setModel(const QString& iName, const QJsonDocument &iDatabase)
    {
        _model = new QJsonTableModel(iName, QJsonDocument(iDatabase), this);
        setModel(_model);
    }

    const QAbstractItemModel *TableView::getModel() const noexcept
    {
        return _model;
    }

    void TableView::setModel(QAbstractItemModel *model)
    {
        QTableView::setModel(_model = qobject_cast<QJsonTableModel*>(model));
        sortByColumn(0, Qt::SortOrder::AscendingOrder); // Может падать
        adjustSize();
    }

//    void TableView::resizeEvent(QResizeEvent *event)
//    {
//        QTableView::resizeEvent(event);
//        adjustSize();
//    }

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
        record.insert(Client::Employee::id(), _model->size());

        const auto fieldNames = Client::Employee::getFieldNames();
        for (int i = 1, I = fieldNames.size(); i < I; ++i)
        {
            QPair<QString, QString> field = fieldNames[i];
            bool result = false;
            std::function<void(QWidget*)> handleLineEdit = [&](QWidget* widget)
            {
                if (widget)
                {
                    QString value;
                    if (auto lineEdit = qobject_cast<const QLineEdit*>(widget))
                    {
                        value = lineEdit->text();
                    }
                    else if (auto comboBox = qobject_cast<const QComboBox*>(widget))
                    {
                        value = comboBox->currentText();
                    }

                    if (_model->checkField(_model->size(), i, value))
                    {
                        record.insert(field.first, value);
                        QPalette palette = widget->palette();
                        palette.setColor(QPalette::Base, Qt::white);
                        palette.setColor(QPalette::Text, Qt::black);
                        widget->setPalette(palette);
                        result = true;
                    }
                    else
                    {
                        QPalette palette = widget->palette();
                        palette.setColor(QPalette::Base, Qt::red);
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
                    if (!_model->deleteRow(selectedRow.row()))
                        return false;
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
