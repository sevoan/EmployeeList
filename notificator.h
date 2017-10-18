#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QWidget>

namespace Ui {
    class Notificator;
}

class NotificatorPrivate;

class Notificator : public QWidget
{
    Q_OBJECT
public:
    explicit Notificator(QWidget *parent = 0);
    virtual ~Notificator();

    void setAlignment(Qt::Alignment alignment);

    bool isAutoHide() const;
    void setAutoHide(bool isAutoHide);

public Q_SLOTS:
    void hideNotify();
    void notify(const QString& message);

protected:
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    Ui::Notificator *ui;
    NotificatorPrivate* d_ptr;
    Q_DECLARE_PRIVATE(Notificator)
};

#endif // NOTIFICATOR_H
