#ifndef PERSONALDATATABLE_H
#define PERSONALDATATABLE_H

#include <QWidget>

class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QJsonArray;

class PersonalDataTable final : public QWidget
{
    Q_OBJECT

public:
    PersonalDataTable(const QString& iName, const QJsonDocument &iData, const QJsonDocument &iPermissions,
                      QGridLayout *iLayout, QGroupBox *iGroupBox, QWidget *parent = nullptr);

    ~PersonalDataTable();

    enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

    void setEditStrategy(EditStrategy iStrategy);
    EditStrategy editStrategy() const { return _strategy; }
    void submitAll();

Q_SIGNALS:
    void sendRequest(const QByteArray &iRequest);

private:
    void update();

private:
    QString _name;
    EditStrategy _strategy = OnFieldChange;
    QGridLayout *_layout;
    QGroupBox *_groupBox;
    QList<QPair<QLabel*, QLineEdit*>> _data;
    std::unique_ptr<QJsonArray> _recordsCache;
};

#endif // PERSONALDATATABLE_H
