#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QWindow>

#include "ui_notificator.h"
#include "notificator_p.h"
#include "notificator.h"

#define TAG "[Notificator]"

#define QUEUE_TIMEOUT           3000    // ms
#define ANIMATION_DURATION      150     // ms
#define NOTIFICATOR_HEIGHT      60      // px

NotificatorPrivate::NotificatorPrivate(Notificator* qq) :
    q_ptr(qq),
    align(Qt::AlignBottom),
    notifyKillTimer(-1),
    isAutoHide(true)
{
}

NotificatorPrivate::~NotificatorPrivate()
{
}

void NotificatorPrivate::init()
{
    Q_Q(Notificator);

    animator.setTargetObject(this);
    animator.setDirection(QPropertyAnimation::Backward);
    animator.setStartValue(positionOffset);
    animator.setPropertyName("translation");
    animator.setDuration(ANIMATION_DURATION);
    animator.setEasingCurve(QEasingCurve::OutCubic);

    connect(&animator, &QPropertyAnimation::stateChanged, [this, q]
            (QPropertyAnimation::State newState, QPropertyAnimation::State) {

        bool directionForward = animator.direction() == QPropertyAnimation::Forward;
        bool directionBackward = animator.direction() == QPropertyAnimation::Backward;
        bool animationStopped = newState == QPropertyAnimation::Stopped;

        if (animationStopped && directionBackward)
            // Animation has stopped and it set notificator to hidden state
            handleNotifyRequest();

        if (isAutoHide && animationStopped && directionForward)
            // Animation has set notificator to completely visible state
            notifyKillTimer = startTimer(QUEUE_TIMEOUT);
    });

    connect(q->ui->button, &QPushButton::clicked, q, &Notificator::hideNotify);
}

void NotificatorPrivate::handleNotifyRequest()
{
    Q_Q(Notificator);

    bool directionForward = animator.direction() == QPropertyAnimation::Forward;
    bool runningForward = animator.state() == QPropertyAnimation::Running && directionForward;
    bool stoppedForward = animator.state() == QPropertyAnimation::Stopped && directionForward;

    if (runningForward || stoppedForward) {
        // Schedule the hide event when animation finished
        bool killTimerActive = notifyKillTimer == -1;
        if (runningForward && !killTimerActive) {
            // Hide notify when animation stopped and will be shown about 2 sec
            int timeLeft = ANIMATION_DURATION - animator.currentTime();
            notifyKillTimer = startTimer(QUEUE_TIMEOUT + timeLeft);
        } else if (stoppedForward) {
            // When notify visible and animation stopped then just hide notify
            notifyKillTimer = startTimer(QUEUE_TIMEOUT);
        }
        return;
    }

    if (messageQueue.isEmpty()) {
        // Prevent blinking animation
        animator.stop();
        return;
    }

    q->ui->messageLabel->setText(messageQueue.takeFirst());

    animator.setStartValue(positionOffset);
    animator.setEndValue(QPoint(0, NOTIFICATOR_HEIGHT));
    animator.setDirection(QPropertyAnimation::Forward);
    animator.start();
}

QPoint NotificatorPrivate::translation() const
{
    return positionOffset;
}

void NotificatorPrivate::setTranslation(const QPoint& translation)
{
    Q_Q(Notificator);

    positionOffset = translation;

    if (align == Qt::AlignBottom)
        q->move(0, q->window()->height() - translation.y());
    else if (align == Qt::AlignTop)
        q->move(0, translation.y());
}

void NotificatorPrivate::timerEvent(QTimerEvent* event)
{
    Q_Q(Notificator);

    if (notifyKillTimer == event->timerId())
        q->hideNotify();
}

Notificator::Notificator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Notificator),
    d_ptr(new NotificatorPrivate(this))
{
    Q_D(Notificator);
    ui->setupUi(this);
    d->init();

}

Notificator::~Notificator()
{
    delete ui;
}

void Notificator::setAlignment(Qt::Alignment alignment)
{
    Q_D(Notificator);
    d->align = alignment;
}

void Notificator::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Notificator::resizeEvent(QResizeEvent* event)
{
    Q_D(Notificator);

    resize(event->size().width(), NOTIFICATOR_HEIGHT);
    move(0, window()->height() - d->positionOffset.y());
}

void Notificator::hideNotify()
{
    Q_D(Notificator);

    if (d->notifyKillTimer != -1) {
        d->killTimer(d->notifyKillTimer);
        d->notifyKillTimer = -1;
    }

    d->animator.stop();
    d->animator.setDirection(QPropertyAnimation::Backward);
    d->animator.start();
}

void Notificator::notify(const QString& message)
{
    Q_D(Notificator);

    if (parent() == Q_NULLPTR) {
        qDebug() << TAG << "Warning: parent isn't set";
        return;
    }

    d->messageQueue.append(message);
    d->handleNotifyRequest();
}

bool Notificator::isAutoHide() const
{
    Q_D(const Notificator);
    return d->isAutoHide;
}

void Notificator::setAutoHide(bool isAutoHide)
{
    Q_D(Notificator);
    d->isAutoHide = isAutoHide;
}
