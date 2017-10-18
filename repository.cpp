#include "employeefilter.h"
#include "repository_p.h"
#include "sqlshortcut.h"
#include "repository.h"
#include "weekload.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QStringBuilder>
#include <QSqlTableModel>

#define TAG         "[Repository]"
#define DB_NAME     "employee.db"

Q_DECLARE_METATYPE(Gender)
Q_DECLARE_METATYPE(PositionType)

const QString CREATE_TEACHERS_TABLE =
        QStringLiteral("CREATE VIRTUAL TABLE IF NOT EXISTS ") + Table::Employee +
        QStringLiteral(" USING fts4("
            "name, birth_year, gender, nationality_id, position_name, education,"
            "specialization, position_type, experience, works_from_year, teaches,"
            "studing_in_absentia, retraining_info, conclusions"
        ")");

const QString CREATE_LOAD_TABLE =
        QStringLiteral("CREATE VIRTUAL TABLE IF NOT EXISTS ") + Table::WeekLoad +
        QStringLiteral(" USING fts4(employee_id, subject, class1_4, class5_9, class10_11)");

const QString CREATE_NATIONALITY_TABLE =
        QStringLiteral("CREATE VIRTUAL TABLE IF NOT EXISTS ") + Table::Nationality +
        QStringLiteral(" USING fts4(value STRING)");

const QString CREATE_QUERY_SHORTCUT_TABLE =
        QStringLiteral("CREATE TABLE IF NOT EXISTS ") + Table::SqlShortcut +
        QStringLiteral(" (id INTEGER PRIMARY KEY AUTOINCREMENT, name STRING, sql STRING)");

RepositoryPrivate::RepositoryPrivate(Repository* qq)
    : q_ptr(qq)
{
}

void RepositoryPrivate::init()
{
    Q_Q(Repository);

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(DB_NAME);

    bool opened = database.open();
    if (!opened) {
        emit q->error(tr("Can't open database: %1 | %2")
                      .arg(DB_NAME, database.lastError().text()));
        return;
    }

    createInitialTables();
}

void RepositoryPrivate::createInitialTables()
{
    Q_Q(Repository);

    QSqlQuery res;
    QSqlError err;

    res = database.exec(CREATE_TEACHERS_TABLE);
    err = res.lastError();
    if (err.isValid()) {
        auto message = tr("Cannot create table %1: error: %2")
                       .arg(Table::Employee, err.text());
        emit q->error(message);
        return;
    }

    res = database.exec(CREATE_LOAD_TABLE);
    err = res.lastError();
    if (err.isValid()) {
        auto message = tr("Cannot create table %1: error: %2")
                       .arg(Table::WeekLoad, err.text());
        emit q->error(message);
        return;
    }

    res = database.exec(CREATE_NATIONALITY_TABLE);
    err = res.lastError();
    if (err.isValid()) {
        auto message = tr("Cannot create table %1: error: %2")
                       .arg(Table::Nationality, err.text());
        emit q->error(message);
        return;
    }

    res = database.exec(CREATE_QUERY_SHORTCUT_TABLE);
    err = res.lastError();
    if (err.isValid()) {
        auto message = tr("Cannot create table %1: error: %2")
                       .arg(Table::SqlShortcut, err.text());
        emit q->error(message);
        return;
    }
}

bool RepositoryPrivate::execAndValidate(QSqlQuery& query, const QString& message) const
{
    Q_Q(const Repository);

    bool executed = query.exec();
    if (!executed) {
        auto warningMessage = tr("%1 %2 | %3")
                       .arg(TAG, query.lastError().text(), query.executedQuery());
        emit q->warning(warningMessage);
        qDebug() << TAG << message << query.lastError().text() << query.executedQuery();
    }

    return executed;
}

void RepositoryPrivate::insertEmployeeWeekLoads(const Employee& employee)
{
    QVariantList employeeIds;
    QVariantList subjectList;
    QVariantList class1_4List;
    QVariantList class5_9List;
    QVariantList class10_11List;

    // Employee also has a week load list. Save it to database
    for (const WeekLoad& weekLoad : employee.weekLoads()) {
        employeeIds     << employee.id();
        subjectList     << weekLoad.subject();
        class1_4List    << weekLoad.class1_4();
        class5_9List    << weekLoad.class5_9();
        class10_11List  << weekLoad.class10_11();
    }

    QSqlQuery loadsQuery(database);
    loadsQuery.prepare(QStringLiteral("INSERT INTO ") + Table::WeekLoad +
                       QStringLiteral(" VALUES (?, ?, ?, ?, ?)"));

    loadsQuery.addBindValue(employeeIds);
    loadsQuery.addBindValue(subjectList);
    loadsQuery.addBindValue(class1_4List);
    loadsQuery.addBindValue(class5_9List);
    loadsQuery.addBindValue(class10_11List);

    loadsQuery.execBatch();
}

void RepositoryPrivate::mapEmployeeToQuery(const Employee& employee, QSqlQuery& query)
{
    query.bindValue(QStringLiteral(":id"),                  employee.id());
    query.bindValue(QStringLiteral(":name"),                employee.name());
    query.bindValue(QStringLiteral(":birth_year"),          employee.birthYear());
    query.bindValue(QStringLiteral(":gender"),              QVariant::fromValue(employee.gender()));
    query.bindValue(QStringLiteral(":nationality_id"),      employee.nationality().id());
    query.bindValue(QStringLiteral(":position_name"),       employee.positionName());
    query.bindValue(QStringLiteral(":education"),           employee.education());
    query.bindValue(QStringLiteral(":specialization"),      employee.specialization());
    query.bindValue(QStringLiteral(":position_type"),       QVariant::fromValue(employee.positionType()));
    query.bindValue(QStringLiteral(":experience"),          employee.experience());
    query.bindValue(QStringLiteral(":works_from_year"),     employee.worksFromYear());
    query.bindValue(QStringLiteral(":teaches"),             employee.teaches());
    query.bindValue(QStringLiteral(":studing_in_absentia"), employee.studingInAbsentia());
    query.bindValue(QStringLiteral(":retraining_info"),     employee.retrainingInformation());
    query.bindValue(QStringLiteral(":conclusions"),         employee.conclusions());
}

Employee RepositoryPrivate::mapQueryToEmployee(const QSqlQuery& query) const
{
    Q_Q(const Repository);

    Employee employee;

    const int columnCount = query.record().count();
    // The last column located in another table and we don't responsible for it
    const int expectedColumnCount = Employee::Column::COLUMN_COUNT - 1;

    if (columnCount < expectedColumnCount) {
        auto count = QString::number(columnCount);
        auto expectedCount = QString::number(expectedColumnCount);

        auto text = tr("%1 Query column count (%2) doesn't equals to Employee(%3). %4")
                    .arg(TAG, count, expectedCount, query.lastQuery());

        emit q->warning(text);

        qDebug() << TAG << "Query column count (" << columnCount
                 << ") doesn't equal to Employee (" << expectedColumnCount << ")";
        qDebug() << query.lastQuery();

        QString fields;

        for (int i = 0; i < query.record().count(); ++i)
            fields += QString::number(i + 1) + "-" + query.record().field(i).name() + " ";

        qDebug() << fields;
    }

    auto id             = query.value(Employee::ID).toLongLong();
    auto name           = query.value(Employee::NAME).toString();
    int birthYear       = query.value(Employee::BIRTH_YEAR).toInt();
    auto gender         = static_cast<Gender>(query.value(Employee::GENDER).toInt());
    auto nationalityId  = query.value(Employee::NATIONALITY).toLongLong();
    auto nationality    = q->nationalityById(nationalityId);
    auto positionName   = query.value(Employee::POSITION_NAME).toString();
    auto positionType   = static_cast<PositionType>(query.value(Employee::POSITION_TYPE).toInt());
    auto education      = query.value(Employee::EDUCATION).toString();
    auto specialization = query.value(Employee::SPECIALIZATION).toString();
    int experience      = query.value(Employee::EXPERIENCE).toInt();
    int worksFromYear   = query.value(Employee::WORKS_FROM_YEAR).toInt();
    auto teaches        = query.value(Employee::TEACHES).toString();
    auto studInAbsentia = query.value(Employee::STUDING_IN_ABSENTIA).toString();
    auto retrainingInfo = query.value(Employee::RETRAINING_INFO).toString();
    auto conclusions    = query.value(Employee::CONCLUSIONS).toString();
    auto weekLoads      = q->weekLoadsById(id);

    employee.setId(id);
    employee.setName(name);
    employee.setBirthYear(birthYear);
    employee.setGender(gender);
    employee.setNationality(nationality);
    employee.setPositionName(positionName);
    employee.setPositionType(positionType);
    employee.setEducation(education);
    employee.setSpecialization(specialization);
    employee.setExperience(experience);
    employee.setWorksFromYear(worksFromYear);
    employee.setTeaches(teaches);
    employee.setStudingInAbsentia(studInAbsentia);
    employee.setRetrainingInfo(retrainingInfo);
    employee.setConclusions(conclusions);
    employee.setWeekLoads(weekLoads);

    return employee;
}

void RepositoryPrivate::mapNationalityToQuery(const Nationality& nationality, QSqlQuery& query)
{
    query.bindValue(":id",      nationality.id());
    query.bindValue(":value",   nationality.value());
}

Nationality RepositoryPrivate::mapQueryToNationality(const QSqlQuery& query) const
{
    Nationality nationality;

    auto id     = query.value(0).toLongLong();
    auto value  = query.value(1).toString();

    nationality.setId(id);
    nationality.setValue(value);

    return nationality;
}

void RepositoryPrivate::mapSqlShortcutToQuery(const SqlShortcut& shortcut, QSqlQuery& query)
{
    query.bindValue(QStringLiteral(":id"),      shortcut.id());
    query.bindValue(QStringLiteral(":name"),    shortcut.name());
    query.bindValue(QStringLiteral(":sql"),     shortcut.sql());
}

SqlShortcut RepositoryPrivate::mapQueryToSqlShortcut(const QSqlQuery& query) const
{
    SqlShortcut shortcut;

    auto id     = query.value(0).toLongLong();
    auto name   = query.value(1).toString();
    auto sql    = query.value(2).toString();

    shortcut.setId(id);
    shortcut.setName(name);
    shortcut.setSql(sql);

    return shortcut;
}

void RepositoryPrivate::mapWeekLoadToQuery(const WeekLoad& load, QSqlQuery& query)
{
    query.bindValue(QStringLiteral(":subject"),     load.subject());
    query.bindValue(QStringLiteral(":class1_4"),    load.class1_4());
    query.bindValue(QStringLiteral(":class5_9"),    load.class5_9());
    query.bindValue(QStringLiteral(":class10_11"),  load.class10_11());
}

WeekLoad RepositoryPrivate::mapQueryToWeekLoad(const QSqlQuery& query) const
{
    WeekLoad load;

    auto subject    = query.value(1).toString();
    int class1_4    = query.value(2).toInt();
    int class5_9    = query.value(3).toInt();
    int class10_11  = query.value(4).toInt();

    load.setSubject(subject);
    load.setClass1_4(class1_4);
    load.setClass5_9(class5_9);
    load.setClass10_11(class10_11);

    return load;
}

void RepositoryPrivate::deleteEmployeeWeekLoads(const Employee& employee)
{
    QSqlQuery query(database);
    query.prepare(QStringLiteral("DELETE FROM ") + Table::WeekLoad +
                  QStringLiteral(" WHERE employee_id = :id"));

    query.bindValue(QStringLiteral(":id"), employee.id());
    execAndValidate(query, Q_FUNC_INFO);
}

Repository::Repository() :
    d_ptr(new RepositoryPrivate(this))
{
}

Repository::~Repository()
{
    delete d_ptr;
}

void Repository::init()
{
    Q_D(Repository);
    d->init();
}

Nationality Repository::nationalityById(qlonglong id) const
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    query.prepare(QStringLiteral("SELECT rowid, * FROM ") + Table::Nationality +
                  QStringLiteral(" WHERE rowid = :id"));

    query.bindValue(":id", id);

    if (d->execAndValidate(query, Q_FUNC_INFO) && query.next())
        return d->mapQueryToNationality(query);

    return Nationality();
}

QVector<WeekLoad> Repository::weekLoadsById(qlonglong id) const
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    QString sql = QStringLiteral("SELECT * FROM ") + Table::WeekLoad +
                  QStringLiteral(" WHERE employee_id = :id");

    query.prepare(sql);
    query.bindValue(":id", id);

    QVector<WeekLoad> weekLoads;

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return weekLoads;

    while (query.next())
        weekLoads << d->mapQueryToWeekLoad(query);

    return weekLoads;
}

QVector<Nationality> Repository::nationalities() const
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    query.prepare(QStringLiteral("SELECT rowid, * FROM ") + Table::Nationality);

    QVector<Nationality> nationalities;

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return nationalities;

    while (query.next())
        nationalities << d->mapQueryToNationality(query);

    return nationalities;
}

void Repository::appendNationality(const Nationality& nationality)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    auto sql = QStringLiteral("INSERT INTO ") + Table::Nationality +
               QStringLiteral(" (value) VALUES (:value)");

    query.prepare(sql);
    d->mapNationalityToQuery(nationality, query);
    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return;

    auto nationalityId = query.lastInsertId().toLongLong();
    const_cast<Nationality&>(nationality).setId(nationalityId);

    emit nationalityAppended(nationality);
}

void Repository::removeNationality(const Nationality& nationality)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    auto sql = QStringLiteral("DELETE FROM ") + Table::Nationality +
               QStringLiteral(" WHERE rowid = :id");

    query.prepare(sql);
    d->mapNationalityToQuery(nationality, query);
    d->execAndValidate(query, Q_FUNC_INFO);
}

void Repository::updateNationality(const Nationality& nationality)
{
    Q_D(Repository);
    QSqlQuery query(d->database);
    auto sql = QStringLiteral("UPDATE ") + Table::Nationality +
               QStringLiteral(" SET value = :value WHERE rowid = :id");

    query.prepare(sql);
    d->mapNationalityToQuery(nationality, query);
    d->execAndValidate(query, Q_FUNC_INFO);
}

QVector<Employee> Repository::employees() const
{
    Q_D(const Repository);
    QVector<Employee> employees;

    QSqlQuery query(d->database);
    query.prepare(QStringLiteral("SELECT rowid,* FROM ") + Table::Employee);

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return employees;

    while (query.next())
        employees << d->mapQueryToEmployee(query);

    return employees;
}

void Repository::appendEmployee(const Employee& employee)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    auto sql = QStringLiteral("INSERT INTO ") + Table::Employee +
               QStringLiteral("(name, birth_year, gender, nationality_id, position_name, education, specialization, position_type,"
                              " experience, works_from_year, teaches, studing_in_absentia, retraining_info, conclusions) VALUES "
                              "(:name, :birth_year, :gender, :nationality_id, :position_name, :education, :specialization, :position_type,"
                              " :experience, :works_from_year, :teaches, :studing_in_absentia, :retraining_info, :conclusions)");

    query.prepare(sql);
    d->mapEmployeeToQuery(employee, query);
    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return;

    auto employeeInsertId = query.lastInsertId().toLongLong();
    const_cast<Employee&>(employee).setId(employeeInsertId);

    d->insertEmployeeWeekLoads(employee);

    emit employeeAppended(employee);
}

bool Repository::removeEmployee(const Employee& employee)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    query.prepare(QStringLiteral("DELETE FROM ") + Table::Employee +
                  QStringLiteral(" WHERE rowid = :id"));

    d->mapEmployeeToQuery(employee, query);

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return false;

    emit employeeRemoved(employee);

    // Remove linked week loads
    d->deleteEmployeeWeekLoads(employee);

    return true;
}

bool Repository::updateEmployee(const Employee& employee)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    QString sql = QStringLiteral("UPDATE ") + Table::Employee +
                  QStringLiteral(" SET"
                                 "  name                = :name,"
                                 "  birth_year          = :birth_year,"
                                 "  gender              = :gender,"
                                 "  nationality_id      = :nationality_id,"
                                 "  position_name       = :position_name,"
                                 "  education           = :education,"
                                 "  specialization      = :specialization,"
                                 "  position_type       = :position_type,"
                                 "  experience          = :experience,"
                                 "  works_from_year     = :works_from_year,"
                                 "  teaches             = :teaches,"
                                 "  studing_in_absentia = :studing_in_absentia,"
                                 "  retraining_info     = :retraining_info,"
                                 "  conclusions         = :conclusions "
                                 "WHERE rowid = :id");

    query.prepare(sql);
    d->mapEmployeeToQuery(employee, query);

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return false;

    // Refresh old loads linked to this employee
    d->deleteEmployeeWeekLoads(employee);
    d->insertEmployeeWeekLoads(employee);

    emit employeeUpdated(employee);
    return true;
}

QVector<SqlShortcut> Repository::sqlShortcuts() const
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    QString sql = QStringLiteral("SELECT * FROM ") + Table::SqlShortcut;
    query.prepare(sql);

    QVector<SqlShortcut> sqlShortcuts;

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return sqlShortcuts;

    while (query.next())
        sqlShortcuts << d->mapQueryToSqlShortcut(query);

    return sqlShortcuts;
}

bool Repository::appendSqlShortcut(const SqlShortcut& shortcut)
{
    Q_D(Repository);

    QSqlQuery query(d->database);
    QString sql = QStringLiteral("INSERT INTO ") + Table::SqlShortcut +
                  QStringLiteral("(name, sql) VALUES (:name, :sql)");

    query.prepare(sql);

    d->mapSqlShortcutToQuery(shortcut, query);

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return false;

    auto shortcutId = query.lastInsertId().toLongLong();
    const_cast<SqlShortcut&>(shortcut).setId(shortcutId);

    emit sqlShortcutAppended(shortcut);

    return true;
}

bool Repository::removeSqlShortcut(const SqlShortcut& shortcut)
{
    Q_D(Repository);

    auto sql = QStringLiteral("DELETE FROM ") + Table::SqlShortcut +
               QStringLiteral(" WHERE id = ") + QString::number(shortcut.id());

    QSqlQuery query(d->database);
    query.prepare(sql);
    d->mapSqlShortcutToQuery(shortcut, query);
    bool valid = d->execAndValidate(query, Q_FUNC_INFO);

    if (valid)
        emit sqlShortcutRemoved(shortcut);

    return valid;
}

bool Repository::updateSqlShortcut(const SqlShortcut& shortcut)
{
    Q_D(Repository);

    QString sql = QStringLiteral("UPDATE ") + Table::SqlShortcut +
                  QStringLiteral(" SET name = :name, sql = :sql WHERE rowid = :id");

    QSqlQuery query(d->database);
    query.prepare(sql);
    d->mapSqlShortcutToQuery(shortcut, query);
    bool valid = d->execAndValidate(query, Q_FUNC_INFO);

    if (valid)
        emit sqlShortcutUpdated(shortcut);

    return valid;
}

QSqlQuery Repository::rawQuery(const QString& sql) const
{
    Q_D(const Repository);
    return QSqlQuery(sql, d->database);
}

bool Repository::isNationalityInUse(const Nationality& nationality) const
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    QString sql = QStringLiteral("SELECT rowid FROM ") + Table::Employee +
                  QStringLiteral(" WHERE nationality_id = :id");

    query.prepare(sql);
    query.bindValue(QStringLiteral(":id"), nationality.id());

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return false;

    return query.next();
}

QVector<Employee> Repository::filterEmployees(EmployeeFilter* filter) const
{
    Q_D(const Repository);

    QString textString;
    QVector<Employee> employeeList;

    // TODO: maybe passing only param list ?
    for (auto param : filter->searchParamList)
        textString += param + "* OR ";

    // Remove last ' OR ' (4 symbols)
    textString = textString.left(textString.size() - 4);

    {
        auto sql = QStringLiteral("SELECT rowid,* FROM ") + Table::Employee +
                   QStringLiteral(" WHERE employee MATCH '") + textString +
                   QStringLiteral("'");

        QSqlQuery query(d->database);
        query.prepare(sql);

        if (!d->execAndValidate(query, Q_FUNC_INFO))
            return employeeList;

        while (query.next())
            employeeList << d->mapQueryToEmployee(query);
    }
    {
        // Now search in nationality
        auto sql = QStringLiteral("SELECT e.rowid,* FROM employee AS e, nationality AS n "
                                  "WHERE nationality MATCH '") + textString +
                   QStringLiteral("' AND e.nationality_id = n.rowid");

        QSqlQuery query(d->database);
        query.prepare(sql);

        if (!d->execAndValidate(query, Q_FUNC_INFO))
            return employeeList;

        while (query.next()) {
            auto empl = d->mapQueryToEmployee(query);
            if (!employeeList.contains(empl))
                employeeList << empl;
        }
    }
    {
        // Now search in loads
        auto sql = QStringLiteral("SELECT e.rowid,* FROM employee AS e, load AS l "
                                  "WHERE load MATCH '") + textString +
                   QStringLiteral("' AND e.rowid = l.employee_id");

        QSqlQuery query(d->database);
        query.prepare(sql);

        if (!d->execAndValidate(query, Q_FUNC_INFO))
            return employeeList;

        while (query.next()) {
            auto empl = d->mapQueryToEmployee(query);
            if (!employeeList.contains(empl))
                employeeList << empl;
        }
    }

    return employeeList;
}

void Repository::filterEmployees(const QString& sql)
{
    Q_D(const Repository);

    QSqlQuery query(d->database);
    query.prepare(sql.isEmpty() ? "SELECT rowid,* FROM employee" : sql);

    if (!d->execAndValidate(query, Q_FUNC_INFO))
        return;

    QVector<Employee> employees;

    while (query.next())
        employees << d->mapQueryToEmployee(query);

    emit employeesChanged(employees);
}

Repository::Repository(RepositoryPrivate& rp)
{
    d_ptr = &rp; // ???
}
