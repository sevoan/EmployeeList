#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

#include "employeefilter.h"

#define TAG "[EmployeeFilter]"

EmployeeFilter::EmployeeFilter()
{
}

EmployeeFilter::EmployeeFilter(const QString& filter)
{
    setFilter(filter);
}

void EmployeeFilter::setFilter(const QString& filter)
{
    searchParamList.clear();

    // Minimin 3 symbols to search
    if (filter.size() < 3) {
        clearFilters();
        return;
    }

    searchParamList = filter.trimmed().split(' ', QString::SkipEmptyParts);

    emit filtersChanged();
}

void EmployeeFilter::clearFilters()
{
    searchParamList.clear();
    emit filtersRemoved();
}
