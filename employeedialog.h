#ifndef EMPLOYEEDIALOG_H
#define EMPLOYEEDIALOG_H

#include <QIcon>
#include <QDialog>

#include "weekload.h"

class Employee;
class EmployeeManager;
class NationalityManager;

namespace Ui {
    class EmployeeDialog;
}

class EmployeeDialogPrivate;

class EmployeeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EmployeeDialog(QWidget *parent = 0);
    virtual ~EmployeeDialog();

    void setEmployeeManager(EmployeeManager* m);
    void setNationalityManager(NationalityManager* m);

    void showEditEmployee(const Employee& employee);
    void showAddEmployee();

public Q_SLOTS:
    void show();
    void hide();

private:
    Ui::EmployeeDialog *ui;
    EmployeeDialogPrivate* d_ptr;
    Q_DECLARE_PRIVATE(EmployeeDialog)
};

#endif // EMPLOYEEDIALOG_H
