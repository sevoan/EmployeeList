#ifndef NATIONALITYDIALOG_H
#define NATIONALITYDIALOG_H

#include <QDialog>

#include "nationality.h"
#include "notificator.h"
#include "formvalidator.h"

namespace Ui {
    class NationalityDialog;
}

class NationalityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NationalityDialog(QWidget *parent = 0);
    ~NationalityDialog();

    void setNationalityManager(NationalityManager* m);

private Q_SLOTS:
    void addNationalityButtonClicked();
    void removeNationalityButtonClicked();
    void nationalityValueUpdated(int row, int column);

protected:
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    void setNationalityAt(int rowIndex, const Nationality& nationality);

private:
    Ui::NationalityDialog *ui;
    NationalityManager* mNationalityManager;
    bool mSelfChangeGuardEnabled;

    Notificator mNotifier;
    FormValidator mFormValidator;
};

#endif // NATIONALITYDIALOG_H
