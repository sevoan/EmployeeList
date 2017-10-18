#ifndef NOTIFICATORPRIVATE_H
#define NOTIFICATORPRIVATE_H

#include <QObject>
#include <QPoint>
#include <QPropertyAnimation>

class Notificator;

class NotificatorPrivate : public QObject {
    Q_OBJECT
    Q_PROPERTY(QPoint translation READ translation WRITE setTranslation)

public:
    NotificatorPrivate(Notificator* qq);
    virtual ~NotificatorPrivate();
    Notificator* q_ptr;

    void init();
    void handleNotifyRequest();

    QPoint translation() const;
    void setTranslation(const QPoint& pt);

protected:
    virtual void timerEvent(QTimerEvent*event) Q_DECL_OVERRIDE;

private:
    Qt::Alignment align;
    QPropertyAnimation animator;
    QVector<QString> messageQueue;
    int notifyKillTimer;
    QPoint positionOffset;
    bool isAutoHide;

    Q_DECLARE_PUBLIC(Notificator)
};

#endif // NOTIFICATORPRIVATE_H
