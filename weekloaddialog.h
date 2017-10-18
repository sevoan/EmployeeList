#ifndef WEEKLOADDIALOG_H
#define WEEKLOADDIALOG_H

#include <QDialog>

#include "formvalidator.h"

namespace Ui {
    class WeekLoadDialog;
}

class WeekLoad;

class WeekLoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WeekLoadDialog(QWidget *parent = 0);
    virtual ~WeekLoadDialog();

public Q_SLOTS:
    void show();

Q_SIGNALS:
    void addWeekLoad(const WeekLoad& load);

private Q_SLOTS:
    void saveButtonClicked();
    void resetFields();

private:
    Ui::WeekLoadDialog *ui;
    FormValidator formValidator;
};

#endif // WEEKLOADDIALOG_H
