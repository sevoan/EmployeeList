#include <QDebug>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_weekloaddialog.h"

#include "weekload.h"
#include "weekloaddialog.h"
#include "formvalidatorrules.h"

WeekLoadDialog::WeekLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeekLoadDialog)
{
    ui->setupUi(this);

    formValidator.appendView(ui->subjectNameEdit, FormValidatorRules::lineEditRules());

    connect(this, &WeekLoadDialog::rejected, this, &WeekLoadDialog::resetFields);
    connect(this, &WeekLoadDialog::rejected, &formValidator, &FormValidator::stopValidation);
}

WeekLoadDialog::~WeekLoadDialog()
{
    delete ui;
}

void WeekLoadDialog::show()
{
    QWidget::show();
    formValidator.startValidation();
    ui->subjectNameEdit->setFocus();
}

void WeekLoadDialog::saveButtonClicked()
{
    if (!formValidator.validateAll())
        return;

    WeekLoad weekLoad;

    weekLoad.setSubject(ui->subjectNameEdit->text());
    weekLoad.setClass1_4(ui->class1_4Box->value());
    weekLoad.setClass5_9(ui->class5_9Box->value());
    weekLoad.setClass10_11(ui->class10_11Box->value());

    emit addWeekLoad(weekLoad);

    if (ui->doNotCloseWindowcheckBox->isChecked())
        resetFields();
    else
        close();
}

void WeekLoadDialog::resetFields()
{
    // Prevent validation on form cleaning
    formValidator.stopValidation();

    ui->subjectNameEdit->clear();
    ui->class1_4Box->setValue(ui->class1_4Box->minimum());
    ui->class5_9Box->setValue(ui->class5_9Box->minimum());
    ui->class10_11Box->setValue(ui->class10_11Box->minimum());

    formValidator.startValidation();

    // Set focus to the first input
    ui->subjectNameEdit->setFocus();
}
