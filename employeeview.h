#ifndef EMPLOYEETABLEVIEW_H
#define EMPLOYEETABLEVIEW_H

#include <QTableView>

class EmployeeDialog1;
class EmployeeViewPrivate;

class EmployeeView : public QWidget
{
public:
    EmployeeView(QWidget* parent = Q_NULLPTR);
    virtual ~EmployeeView();

    void setEmployeeFormVisible(bool visible);
    bool isEmployeeFormVisible() const;

public Q_SLOTS:
    void showEmployeeForm();
    void hideEmployeeForm();

protected:
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    EmployeeViewPrivate* d_ptr;
    Q_DECLARE_PRIVATE(EmployeeView)
};

#endif // EMPLOYEETABLEVIEW_H
