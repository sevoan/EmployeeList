#ifndef EMPLOYEEFORMPRIVATE_H
#define EMPLOYEEFORMPRIVATE_H

#include <QObject>

#include "employeedialog.h"
#include "weekloaddialog.h"

class Employee;
class WeekLoad;
class QComboBox;
class EmployeeDialog;
class EmployeeManager;
class WeekLoadDialog;
class NationalityManager;
class FormValidator;

class EmployeeDialogPrivate : public QObject {
    Q_OBJECT
  public:
    enum class Type {
        ADD,
        EDIT
    };

    EmployeeDialogPrivate(EmployeeDialog* qq);
    ~EmployeeDialogPrivate();

    EmployeeDialog* q_ptr;

    Type dialogType;
    EmployeeManager* employeeManager;
    NationalityManager* nationalityManager;
    QScopedPointer<WeekLoadDialog> weekLoadDialog;
    QScopedPointer<FormValidator> formValidator;

    void init();
    void resetFields();
    void initFieldsByDefault();
    Employee mapEmployeeInputs();

    template<class T>
    void setComboboxActiveData(QComboBox* box, T data);

public Q_SLOTS:
    void saveOrUpdate();
    void showWeekLoadDialog();
    void removeSelectedWeekLoad();
    void addWeekLoad(const WeekLoad& load);

    Q_DECLARE_PUBLIC(EmployeeDialog)
};

#endif // EMPLOYEEFORMPRIVATE_H
