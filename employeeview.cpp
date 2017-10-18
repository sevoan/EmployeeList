#include <QDebug>
#include <QResizeEvent>
#include <QPainter>

#include "employeeview_p.h"
#include "employeeview.h"

EmployeeViewPrivate::EmployeeViewPrivate(EmployeeView* q) :
    q_ptr(q),
    employeeTableView(new QTableView(q)),
//    employeeForm(new EmployeeDialog1(q)),
    backCloack(new Cloak(q)),
    isFormGoingToHide(false),
    isFormAnimationRunning(false)
{
}

void EmployeeViewPrivate::init()
{
    Q_Q(EmployeeView);

    employeeTableView->setFrameShape(QFrame::NoFrame);

//    employeeForm->hide();
//    employeeForm->raise();

    auto palette = employeeForm->palette();
    palette.setColor(QPalette::Background, Qt::white);
//    employeeForm->setAutoFillBackground(true);
//    employeeForm->setPalette(palette);

    connect(backCloack.data(), &Cloak::pressClicked, q, &EmployeeView::hideEmployeeForm);

    setupAnimations();
}

void EmployeeViewPrivate::setupAnimations()
{
    formAnimator.setTargetObject(employeeForm.data());
    formAnimator.setPropertyName("x");
    formAnimator.setEasingCurve(QEasingCurve::InOutQuart);
    formAnimator.setDuration(150);

    connect(&formAnimator, &QPropertyAnimation::finished, [this] () {
//        if (isFormGoingToHide)
//            employeeForm->hide();
    });
}

void EmployeeViewPrivate::setFormVisible(bool visible)
{
    Q_Q(EmployeeView);

    isFormGoingToHide = !visible;

//    int endX = q->width();
//    if (visible)
//        endX -= employeeForm->width();

//    if (visible)
//        employeeForm->show();

//    formAnimator.setEasingCurve(QEasingCurve::InOutQuad);
//    formAnimator.setStartValue(employeeForm->x());
//    formAnimator.setEndValue(endX);
//    formAnimator.start();

//    backCloack->setElevated(visible);
}

EmployeeView::EmployeeView(QWidget* parent) :
    QWidget(parent),
    d_ptr(new EmployeeViewPrivate(this))
{
    Q_D(EmployeeView);
    d->init();
}

EmployeeView::~EmployeeView()
{
    delete d_ptr;
}

void EmployeeView::showEmployeeForm()
{
    Q_D(EmployeeView);

//    if (d->employeeForm->isVisible() || d->isFormAnimationRunning)
//        return;

    d->setFormVisible(true);
}

void EmployeeView::hideEmployeeForm()
{
    Q_D(EmployeeView);

//    if (d->employeeForm->isHidden() || d->isFormAnimationRunning)
//        return;

    d->setFormVisible(false);
}

void EmployeeView::setEmployeeFormVisible(bool visible)
{
    Q_D(EmployeeView);
    d->setFormVisible(visible);
}

bool EmployeeView::isEmployeeFormVisible() const
{
    Q_D(const EmployeeView);
    return d->employeeForm->isVisible();
}

void EmployeeView::resizeEvent(QResizeEvent* event)
{
    Q_D(EmployeeView);

    const QSize newParentSize = event->size();
    d->employeeTableView->resize(newParentSize);
    d->employeeForm->resize(500, newParentSize.height());

    if (d->isFormAnimationRunning)
        return;

    if (d->employeeForm->isHidden()) {
        d->employeeForm->move(newParentSize.width(), 0);
    } else {
        d->employeeForm->move(newParentSize.width() - 500, 0);
    }

    d->backCloack->resize(newParentSize);
}

void EmployeeView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
}


