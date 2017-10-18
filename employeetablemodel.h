#ifndef EMPLOYEETABLEVIEW_H
#define EMPLOYEETABLEVIEW_H

#include <QTableView>

class Employee;
class Repository;
class EmployeeFilter;

class EmployeeTableModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    EmployeeTableModel(QObject *parent = Q_NULLPTR);
    virtual ~EmployeeTableModel();

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &child = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void setEmployeeFilter(EmployeeFilter* filter);
    void setRepository(Repository* repository);
    Employee employeeAt(int index) const;

private Q_SLOTS:
    void employeeAdded(const Employee& employee);
    void employeeRemoved(const Employee& employee);
    void employeeUpdated(const Employee& employee);

private:
    Repository* mRepository;
    EmployeeFilter* mEmployeeFilter;
    QVector<Employee> mEmployees;

};

#endif // EMPLOYEETABLEVIEW_H
