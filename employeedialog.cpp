#include <QDebug>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QDialogButtonBox>

#include <algorithm>

#include "ui_employeedialog.h"
#include "formvalidator.h"
#include "employeedialog_p.h"
#include "employeedialog.h"
#include "weekloaddialog.h"
#include "formvalidatorrules.h"
#include "nationality.h"
#include "employee.h"

#define LOAD_FIRST_COLUMN_WIDTH    250

Q_DECLARE_METATYPE(Gender)
Q_DECLARE_METATYPE(WeekLoad)
Q_DECLARE_METATYPE(Nationality)
Q_DECLARE_METATYPE(PositionType)

EmployeeDialogPrivate::EmployeeDialogPrivate(EmployeeDialog* qq) :
    q_ptr(qq),
    employeeManager(Q_NULLPTR),
    nationalityManager(Q_NULLPTR),
    weekLoadDialog(new WeekLoadDialog(qq)),
    formValidator(new FormValidator())
{
    connect(weekLoadDialog.data(), &WeekLoadDialog::addWeekLoad,
            this, &EmployeeDialogPrivate::addWeekLoad);
}

EmployeeDialogPrivate::~EmployeeDialogPrivate()
{   
}

void EmployeeDialogPrivate::init()
{
    Q_Q(EmployeeDialog);

    connect(q, &EmployeeDialog::rejected, this, &EmployeeDialogPrivate::resetFields);

    auto ui = q->ui;

    // Setup line edit rules
    auto lineEditRules = FormValidatorRules::lineEditRules();
    auto comboBoxRules = FormValidatorRules::comboBoxRules();

    formValidator->appendView(ui->nameEdit, lineEditRules);
    formValidator->appendView(ui->positionNameEdit, lineEditRules);
    formValidator->appendView(ui->educationEdit, lineEditRules);
    formValidator->appendView(ui->specializationEdit, lineEditRules);

    // Setup combobox
    formValidator->appendView(ui->nationalityBox, comboBoxRules);

    // Begin validate
    formValidator->startValidation();
}

void EmployeeDialogPrivate::initFieldsByDefault()
{
    Q_Q(EmployeeDialog);
    auto ui = q->ui;

    ui->genderBox->addItem(GenderString(Gender::Male), QVariant::fromValue(Gender::Male));
    ui->genderBox->addItem(GenderString(Gender::Female), QVariant::fromValue(Gender::Female));

    if (nationalityManager != Q_NULLPTR) {
        auto nationalities = nationalityManager->nationalities();
        if (nationalities.isEmpty())
            qDebug() << "Warning here's no nationalities";
        for (const Nationality& nationality : nationalities)
            ui->nationalityBox->addItem(nationality.value(),
                                        QVariant::fromValue(nationality));
    } else {
        qDebug() << "Please provide natinality manager to EmployeeDialog";
    }

    ui->positionTypeBox->addItem(PositionTypeString(PositionType::Primary),
                                 QVariant::fromValue(PositionType::Primary));

    ui->positionTypeBox->addItem(PositionTypeString(PositionType::Secondary),
                                 QVariant::fromValue(PositionType::Secondary));

    ui->birthYearEdit->setValue(ui->birthYearEdit->minimum());
    ui->experienceEdit->setValue(ui->experienceEdit->minimum());
    ui->worksFromYearEdit->setValue(ui->worksFromYearEdit->minimum());
}

void EmployeeDialogPrivate::resetFields()
{
    Q_Q(EmployeeDialog);
    auto ui = q->ui;

    formValidator->stopValidation();

    ui->idBox->setValue(-1);
    ui->nameEdit->clear();
    ui->birthYearEdit->clear();
    ui->genderBox->clear();
    ui->nationalityBox->clear();
    ui->positionNameEdit->clear();
    ui->educationEdit->clear();
    ui->specializationEdit->clear();
    ui->positionTypeBox->clear();
    ui->experienceEdit->clear();
    ui->worksFromYearEdit->clear();
    ui->teachesEdit->clear();
    ui->studingInAbsentiaEdit->clear();
    ui->retrainingInformationEdit->clear();
    ui->conclusionsEdit->clear();

    // Reset week load table
    int weekLoadRowCount = ui->weekLoadsTable->rowCount();
    while (weekLoadRowCount--)
        ui->weekLoadsTable->removeRow(weekLoadRowCount);

    formValidator->startValidation();
}

Employee EmployeeDialogPrivate::mapEmployeeInputs()
{
    Q_Q(EmployeeDialog);
    auto ui = q->ui;

    Employee employee;

    employee.setId(ui->idBox->value());
    employee.setName(ui->nameEdit->text());
    employee.setBirthYear(ui->birthYearEdit->value());

    auto gender = ui->genderBox->currentData().value<Gender>();
    employee.setGender(gender);

    auto nationality = ui->nationalityBox->currentData().value<Nationality>();
    employee.setNationality(nationality);

    employee.setPositionName(ui->positionNameEdit->text());
    employee.setEducation(ui->educationEdit->text());
    employee.setSpecialization(ui->specializationEdit->text());

    auto positionType = ui->positionTypeBox->currentData().value<PositionType>();
    employee.setPositionType(positionType);

    employee.setExperience(ui->experienceEdit->value());
    employee.setWorksFromYear(ui->worksFromYearEdit->value());
    employee.setTeaches(ui->teachesEdit->text());
    employee.setStudingInAbsentia(ui->studingInAbsentiaEdit->text());
    employee.setRetrainingInfo(ui->retrainingInformationEdit->text());
    employee.setConclusions(ui->conclusionsEdit->text());

    // Load employee week load from table widget

    QVector<WeekLoad> weekLoadList;

    for (int rowIndex = 0; rowIndex < ui->weekLoadsTable->rowCount(); ++rowIndex) {
        auto subjectItem = ui->weekLoadsTable->item(rowIndex, 0);
        weekLoadList << subjectItem->data(Qt::UserRole).value<WeekLoad>();
    }

    employee.setWeekLoads(weekLoadList);
    return employee;
}

template<class T>
void EmployeeDialogPrivate::setComboboxActiveData(QComboBox* comboBox, T data)
{
    int count = comboBox->count();
    for (int index = 0; index < count; ++index) {
        QVariant underlyingData = comboBox->itemData(index);
        if (underlyingData.value<T>() == data) {
            comboBox->setCurrentIndex(index);
            return;
        }
    }
}

void EmployeeDialogPrivate::saveOrUpdate()
{
    Q_Q(EmployeeDialog);

    bool valid = formValidator->validateAll();
    if (!valid)
        return;

    if (employeeManager == Q_NULLPTR) {
        qDebug() << "employee manager isn't set";
        return;
    }

    Employee employee = mapEmployeeInputs();

    if (dialogType == Type::ADD)
        employeeManager->appendEmployee(employee);
    else if (dialogType == Type::EDIT)
        employeeManager->updateEmployee(employee);

    q->hide();
}

void EmployeeDialogPrivate::showWeekLoadDialog()
{
    weekLoadDialog->show();
}

void EmployeeDialogPrivate::addWeekLoad(const WeekLoad& load)
{
    Q_Q(EmployeeDialog);
    auto ui = q->ui;

    // Put values directly to table widget model
    QTableWidgetItem* subjectName = new QTableWidgetItem(load.subject());
    QTableWidgetItem* class1_4 = new QTableWidgetItem(QString::number(load.class1_4()));
    QTableWidgetItem* class5_9 = new QTableWidgetItem(QString::number(load.class5_9()));
    QTableWidgetItem* class10_11 = new QTableWidgetItem(QString::number(load.class10_11()));

    class1_4->setTextAlignment(Qt::AlignCenter);
    class5_9->setTextAlignment(Qt::AlignCenter);
    class10_11->setTextAlignment(Qt::AlignCenter);

    int newRowIndex = ui->weekLoadsTable->rowCount();

    ui->weekLoadsTable->insertRow(newRowIndex);
    ui->weekLoadsTable->setItem(newRowIndex, 0, subjectName);
    ui->weekLoadsTable->setItem(newRowIndex, 1, class1_4);
    ui->weekLoadsTable->setItem(newRowIndex, 2, class5_9);
    ui->weekLoadsTable->setItem(newRowIndex, 3, class10_11);

    // Save entire model in first column
    subjectName->setData(Qt::UserRole, QVariant::fromValue(load));
}

void EmployeeDialogPrivate::removeSelectedWeekLoad()
{
    Q_Q(EmployeeDialog);
    auto ui = q->ui;

    QVector<int> sortedIndices;
    auto selectedIndices = ui->weekLoadsTable->selectionModel()->selectedRows();
    for (auto index : selectedIndices)
        sortedIndices << index.row();

    std::sort(sortedIndices.begin(), sortedIndices.end(), std::less<int>());

    int index = sortedIndices.size();
    while (index--) {
        int rowIndexToRemove = sortedIndices.at(index);
        ui->weekLoadsTable->removeRow(rowIndexToRemove);
    }
}

EmployeeDialog::EmployeeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmployeeDialog),
    d_ptr(new EmployeeDialogPrivate(this))
{
    Q_D(EmployeeDialog);

    ui->setupUi(this);

    connect(ui->resultButtons, &QDialogButtonBox::accepted,
            d, &EmployeeDialogPrivate::saveOrUpdate);

    // Register signals here because dialog ui can't connect directly to D-Pointer slots
    connect(ui->addLoadButton, &QPushButton::clicked,
            d, &EmployeeDialogPrivate::showWeekLoadDialog);
    connect(ui->removeLoadButton, &QPushButton::clicked,
            d, &EmployeeDialogPrivate::removeSelectedWeekLoad);

    QStringList weekLoadLabels;
    weekLoadLabels << tr("Subject") << tr("1-4") << tr("5-9") << tr("10-11");

    ui->weekLoadsTable->setHorizontalHeaderLabels(weekLoadLabels);
    ui->weekLoadsTable->setColumnWidth(0, LOAD_FIRST_COLUMN_WIDTH);

    d->init();
}

EmployeeDialog::~EmployeeDialog()
{
    delete ui;
    delete d_ptr;
}

void EmployeeDialog::setEmployeeManager(EmployeeManager* manager)
{
    Q_D(EmployeeDialog);
    d->employeeManager = manager;
}

void EmployeeDialog::show()
{
    QWidget::show();
    ui->nameEdit->setFocus();
}

void EmployeeDialog::hide()
{
    Q_D(EmployeeDialog);
    d->resetFields();
    QDialog::hide();
}

void EmployeeDialog::setNationalityManager(NationalityManager* manager)
{
    Q_D(EmployeeDialog);
    d->nationalityManager = manager;
}

void EmployeeDialog::showAddEmployee()
{
    Q_D(EmployeeDialog);
    d->initFieldsByDefault();

    setWindowTitle(tr("Add employee"));
    d->dialogType = EmployeeDialogPrivate::Type::ADD;

    QDialog::show();
}

void EmployeeDialog::showEditEmployee(const Employee& employee)
{
    Q_D(EmployeeDialog);

    d->initFieldsByDefault();
    d->dialogType = EmployeeDialogPrivate::Type::EDIT;
    setWindowTitle(tr("Edit employee"));

    ui->idBox->setValue(employee.id());
    ui->nameEdit->setText(employee.name());
    ui->birthYearEdit->setValue(employee.birthYear());

    // Gender
    d->setComboboxActiveData(ui->genderBox, employee.gender());

    // Nationality
    d->setComboboxActiveData(ui->nationalityBox, employee.nationality());

    ui->positionNameEdit->setText(employee.positionName());
    ui->educationEdit->setText(employee.education());
    ui->specializationEdit->setText(employee.specialization());

    // Position type
    d->setComboboxActiveData(ui->positionTypeBox, employee.positionType());

    ui->experienceEdit->setValue(employee.experience());
    ui->worksFromYearEdit->setValue(employee.worksFromYear());

    ui->teachesEdit->setText(employee.teaches());
    ui->studingInAbsentiaEdit->setText(employee.studingInAbsentia());
    ui->retrainingInformationEdit->setText(employee.retrainingInformation());
    ui->conclusionsEdit->setText(employee.conclusions());

    for (auto weekLoad : employee.weekLoads())
        d->addWeekLoad(weekLoad);

    QDialog::show();
}
