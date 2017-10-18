#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QSqlQueryModel>

#include "employee.h"
#include "nationality.h"
#include "sqlshortcut.h"

class QSqlTableModel;
class EmployeeFilter;
class RepositoryPrivate;

namespace Table {
    static QString Employee         = QStringLiteral("employee");
    static QString WeekLoad         = QStringLiteral("load");
    static QString Nationality      = QStringLiteral("nationality");
    static QString SqlShortcut      = QStringLiteral("query_shortcuts");
}

class Repository : public QObject, public NationalityManager, public EmployeeManager, public SqlShortcutManager
{
    Q_OBJECT
public:
    explicit Repository();
    virtual ~Repository();

    void init();

    QVector<Employee> filterEmployees(EmployeeFilter* filter) const;
    void filterEmployees(const QString& sql = "");

    virtual QVector<WeekLoad> weekLoadsById(qlonglong id) const;
    virtual Nationality nationalityById(qlonglong id) const;

    virtual QVector<Nationality> nationalities() const Q_DECL_OVERRIDE;
    virtual void appendNationality(const Nationality& nationality) Q_DECL_OVERRIDE;
    virtual void removeNationality(const Nationality& nationality) Q_DECL_OVERRIDE;
    virtual void updateNationality(const Nationality &nationality) Q_DECL_OVERRIDE;
    virtual bool isNationalityInUse(const Nationality &nationality) const Q_DECL_OVERRIDE;

    virtual QVector<Employee> employees() const Q_DECL_OVERRIDE;
    virtual void appendEmployee(const Employee& employee) Q_DECL_OVERRIDE;
    virtual bool removeEmployee(const Employee& employee) Q_DECL_OVERRIDE;
    virtual bool updateEmployee(const Employee& employee) Q_DECL_OVERRIDE;

    virtual QVector<SqlShortcut> sqlShortcuts() const Q_DECL_OVERRIDE;
    virtual bool appendSqlShortcut(const SqlShortcut& shortcut) Q_DECL_OVERRIDE;
    virtual bool removeSqlShortcut(const SqlShortcut& shortcut) Q_DECL_OVERRIDE;
    virtual bool updateSqlShortcut(const SqlShortcut& shortcut) Q_DECL_OVERRIDE;

    QSqlQuery rawQuery(const QString& sql) const;

protected:
    Repository(RepositoryPrivate& rp);

Q_SIGNALS:
    void nationalityAppended(const Nationality& employee);
    void nationalityUpdated(const Nationality& employee);
    void nationalityRemoved(const Nationality& employee);

    void employeesChanged(QVector<Employee> employees);
    void employeeAppended(const Employee& employee);
    void employeeUpdated(const Employee& employee);
    void employeeRemoved(const Employee& employee);

    void sqlShortcutAppended(const SqlShortcut& shortcut);
    void sqlShortcutUpdated(const SqlShortcut& shortcut);
    void sqlShortcutRemoved(const SqlShortcut& shortcut);

    void error(const QString& message);
    void warning(const QString& message) const;

private:
    RepositoryPrivate* d_ptr;
    Q_DECLARE_PRIVATE(Repository)

};

#endif // REPOSITORY_H
