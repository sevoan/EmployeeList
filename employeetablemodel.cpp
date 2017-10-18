#include <QDebug>

#include "repository.h"
#include "employeefilter.h"
#include "employeetablemodel.h"

EmployeeTableModel::EmployeeTableModel(QObject* parent) :
    QAbstractItemModel(parent),
    mRepository(Q_NULLPTR),
    mEmployeeFilter(Q_NULLPTR)
{
}

EmployeeTableModel::~EmployeeTableModel()
{
}

QModelIndex EmployeeTableModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex EmployeeTableModel::parent(const QModelIndex& child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int EmployeeTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return mEmployees.size();
}

int EmployeeTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return Employee::COLUMN_COUNT;
}

QVariant EmployeeTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        auto data = mEmployees.at(index.row());
        return data.valueAt(index.column());
    }

    return QVariant();
}

QVariant EmployeeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case Employee::ID:                  return tr("ID");
                case Employee::NAME:                return tr("Name");
                case Employee::BIRTH_YEAR:          return tr("Birth year");
                case Employee::GENDER:              return tr("Gender");
                case Employee::NATIONALITY:         return tr("Nationality");
                case Employee::POSITION_NAME:       return tr("Position name");
                case Employee::EDUCATION:           return tr("Education");
                case Employee::SPECIALIZATION:      return tr("Specialization");
                case Employee::POSITION_TYPE:       return tr("Position type");
                case Employee::EXPERIENCE:          return tr("Experience");
                case Employee::WORKS_FROM_YEAR:     return tr("Works from year");
                case Employee::TEACHES:             return tr("Teaches");
                case Employee::STUDING_IN_ABSENTIA: return tr("Studing in absentia");
                case Employee::RETRAINING_INFO:     return tr("Retraining info");
                case Employee::CONCLUSIONS:         return tr("Conclusions");
                case Employee::WEEK_LOADS:          return tr("Subjects");
                default:                            return QVariant();
            }
        } else if (orientation == Qt::Vertical) {
            return section + 1;
        }
    }

    return QVariant();
}

void EmployeeTableModel::setRepository(Repository* repository)
{
    if (repository == Q_NULLPTR)
        return;

    mRepository = repository;

    connect(repository, &Repository::employeeAppended, this, &EmployeeTableModel::employeeAdded);
    connect(repository, &Repository::employeeUpdated, this, &EmployeeTableModel::employeeUpdated);
    connect(repository, &Repository::employeeRemoved, this, &EmployeeTableModel::employeeRemoved);
    connect(repository, &Repository::employeesChanged, this, [this] (QVector<Employee> employees) {
        mEmployees = employees;
        emit layoutChanged();
    });

    mEmployees = repository->employees();
}

Employee EmployeeTableModel::employeeAt(int index) const
{
    return mEmployees.at(index);
}

void EmployeeTableModel::employeeAdded(const Employee& employee)
{
    int newRowIndex = rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), newRowIndex, newRowIndex);
    mEmployees.append(employee);
    endInsertRows();
}

void EmployeeTableModel::employeeRemoved(const Employee& employee)
{
    int rowToRemove = mEmployees.indexOf(employee);
    if (rowToRemove == -1) {
        qDebug() << "employee row not found:" << employee.id();
        return;
    }

    beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
    mEmployees.remove(rowToRemove);
    endRemoveRows();
}

void EmployeeTableModel::employeeUpdated(const Employee& employee)
{
    int rowToUpdate = mEmployees.indexOf(employee);
    if (rowToUpdate == -1) {
        qDebug() << "employee row not found:" << employee.id();
        return;
    }

    mEmployees[rowToUpdate] = employee;

    QVector<int> roles;
    roles << Qt::DisplayRole;

    auto leftRow = createIndex(rowToUpdate, 0);
    auto rightRow = createIndex(rowToUpdate, columnCount());
    emit dataChanged(leftRow, rightRow, roles);
}

void EmployeeTableModel::setEmployeeFilter(EmployeeFilter* filter)
{
    if (filter == Q_NULLPTR)
        return;

    mEmployeeFilter = filter;

    connect(filter, &EmployeeFilter::filtersChanged, [this] () {
        mEmployees = mRepository->filterEmployees(mEmployeeFilter);
        emit layoutChanged();
    });

    connect(filter, &EmployeeFilter::filtersRemoved, [this] () {
        mEmployees = mRepository->employees();
        emit layoutChanged();
    });
}
