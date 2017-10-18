#ifndef REPOSITORYPRIVATE_H
#define REPOSITORYPRIVATE_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class WeekLoad;
class Employee;
class Repository;
class SqlShortcut;
class Nationality;

class RepositoryPrivate : public QObject {
    Q_OBJECT
public:
    RepositoryPrivate(Repository* qq);
    Repository* q_ptr;

    void init();
    void createInitialTables();

    bool execAndValidate(QSqlQuery& query, const QString& message) const;

    void insertEmployeeWeekLoads(const Employee& employee);
    void deleteEmployeeWeekLoads(const Employee& employee);

    void mapEmployeeToQuery(const Employee& employee, QSqlQuery& query);
    Employee mapQueryToEmployee(const QSqlQuery& query) const;

    void mapNationalityToQuery(const Nationality& nationality, QSqlQuery& query);
    Nationality mapQueryToNationality(const QSqlQuery& query) const;

    void mapSqlShortcutToQuery(const SqlShortcut& shortcut, QSqlQuery& query);
    SqlShortcut mapQueryToSqlShortcut(const QSqlQuery& query) const;

    void mapWeekLoadToQuery(const WeekLoad& load, QSqlQuery& query);
    WeekLoad mapQueryToWeekLoad(const QSqlQuery& query) const;


private:
    QSqlDatabase database;

    Q_DECLARE_PUBLIC(Repository)
};

#endif // REPOSITORYPRIVATE_H
