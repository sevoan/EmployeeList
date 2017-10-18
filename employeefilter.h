#ifndef STRINGREQUESTPARSER_H
#define STRINGREQUESTPARSER_H

#include <QObject>
#include <QString>

#include "employee.h"

class QSqlDatabase;

class EmployeeFilter : public QObject
{
    Q_OBJECT
public:
    EmployeeFilter();
    EmployeeFilter(const QString& filter);

public Q_SLOTS:
    void setFilter(const QString& filter);
    void clearFilters();

Q_SIGNALS:
    void filtersChanged();
    void filtersRemoved();

private:
    QStringList searchParamList;
    friend class Repository;
};

#endif // STRINGREQUESTPARSER_H
