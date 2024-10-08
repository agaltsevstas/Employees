#include "tableView.h"
#include "client.h"
#include "delegate.h"

#include <QCryptographicHash>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonTableModel>
#include <QMessageBox>
#include <QTimer>

#include <ranges>


namespace Client
{
    TableView::TableView(QWidget* parent) : QTableView(parent)
    {
        setObjectName(QString::fromUtf8("tableView"));
        setItemDelegate(new Delegate(this));
        setSortingEnabled(true);
        setMouseTracking(true);
        setMinimumSize(QSize(800, 700));
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

    TableView::EditStrategy TableView::getEditStrategy() const noexcept
    {
        if (_model)
            return static_cast<TableView::EditStrategy>(_model->getEditStrategy());

        return EditStrategy::OnFieldChange;
    }

    void TableView::setModel(const QString& iName, const QJsonDocument& iDatabase, const QJsonDocument& iPermissions)
    {
        setModel(new QJsonTableModel(iName, QJsonDocument(iDatabase), QJsonDocument(iPermissions), this));
    }

    void TableView::setModel(const QString& iName, const QJsonDocument& iDatabase)
    {
        setModel(new QJsonTableModel(iName, QJsonDocument(iDatabase), this));
    }

    const QAbstractItemModel* TableView::getModel() const noexcept
    {
        return _model;
    }

    void TableView::setModel(QAbstractItemModel* model)
    {
        if (_model)
        {
            qInfo() << "Удаление предыдущей модели БД";
            delete _model;
        }

        qInfo() << "Установка новой модели БД";
        QTableView::setModel(_model = qobject_cast<QJsonTableModel*>(model));
        setSortingEnabled(true);
        sortByColumn(0, Qt::SortOrder::AscendingOrder); // Может падать
        horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(10, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(12, QHeaderView::Interactive);
        connect(_model, &QJsonTableModel::sendCreateRequest, this, &TableView::sendCreateData);
        connect(_model, &QJsonTableModel::sendDeleteRequest, this, &TableView::sendDeleteData);
        connect(_model, &QJsonTableModel::sendUpdateRequest, this, &TableView::sendUpdateData);
    }

    void TableView::submitAll()
    {
        if (_model)
            _model->submitAll();
    }

    void TableView::revertAll()
    {
        if (_model)
            _model->revertAll();
        sortByColumn(0, Qt::SortOrder::AscendingOrder);
    }

    bool TableView::addUser()
    {
        if (!_model)
            return false;

        QJsonObject record;
        const auto fieldNames = Client::Employee::getFieldNames();
        for (const auto i : std::views::iota(1, fieldNames.size()))
        {
            bool result = false;
            QPair<QString, QString> field = fieldNames[i];
            emit getUserData(field.first, [&](const QString& iValue)->bool
            {
                if (_model->checkField(_model->rowCount(), i, iValue))
                {
                    if (field.first == Client::Employee::password())
                        record.insert(field.first, QJsonValue::fromVariant(QCryptographicHash::hash(iValue.toUtf8(), QCryptographicHash::Md5).toHex()));
                    else
                        record.insert(field.first, iValue);
                    return result = true;
                }

                return false;
            });

            if (!result)
                return false;
        }

        _model->addRow(record);
        selectionModel()->clearSelection();

        const QString message = "Сотрудник успешно добавлен!";
        QMessageBox information(QMessageBox::Icon::Information, tr("Сообщение"), message, QMessageBox::NoButton, this);
        QTimer::singleShot(1000, &information, &QMessageBox::close);
        information.exec();
        qInfo() << "Сообщение: " << message;
        return true;
    }

    bool TableView::deleteUser()
    {
        if (!_model)
            return false;

        if (QItemSelectionModel* select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto& selectedRow : select->selectedRows())
                {
                    _model->deleteRow(selectedRow.row());
                }

                selectionModel()->clearSelection();
                return true;
            }
        }

        return false;
    }

    void TableView::restoreUser()
    {
        if (!_model)
            return;

        if (QItemSelectionModel* select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto& selectedRow : select->selectedRows())
                {
                    _model->restoreRow(selectedRow.row());
                }
            }
        }
    }

    std::optional<bool> TableView::canDeleteUser()
    {
        if (!_model)
            return {};

        std::optional<bool> allDeleted;

        if (QItemSelectionModel* select = selectionModel())
        {
            if (select->hasSelection())
            {
                for (const auto& selectedRow : select->selectedRows())
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

    bool TableView::checkChanges() const noexcept
    {
        return _model->checkChanges();
    }

    void TableView::valueSearchChanged(const QString& iValue)
    {
        if (!_model)
            return;

        std::ranges::for_each(_hiddenIndices, [this](const int index)
        {
            setRowHidden(index, false);
        });

        _hiddenIndices = _model->search(iValue);
        std::ranges::for_each(_hiddenIndices, [this](const int index)
        {
            setRowHidden(index, true);
        });
    }

    void TableView::clearSearchChanged()
    {
        std::ranges::for_each(_hiddenIndices, [this](const int index)
        {
            setRowHidden(index, false);
        });

        _hiddenIndices.clear();
    }
}
