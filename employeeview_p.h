#ifndef EMPLOYEETABLEVIEWPRIVATE_H
#define EMPLOYEETABLEVIEWPRIVATE_H

#include <QObject>
#include <QTableView>
#include <QPropertyAnimation>

#include "cloak.h"

class EmployeeView;

class EmployeeViewPrivate : public QObject {
    Q_OBJECT
  public:
    EmployeeViewPrivate(EmployeeView* q);
    EmployeeView* q_ptr;

    void init();
    void setupAnimations();
    void setFormVisible(bool visible);

    QScopedPointer<QTableView> employeeTableView;
    QScopedPointer<EmployeeDialog1> employeeForm;
    QScopedPointer<Cloak> backCloack;

    bool isFormGoingToHide;
    bool isFormAnimationRunning;
    QPropertyAnimation formAnimator;

    Q_DECLARE_PUBLIC(EmployeeView)
};

#endif // EMPLOYEETABLEVIEWPRIVATE_H
