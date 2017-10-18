#include <QResizeEvent>
#include <QDebug>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>

#include "ui_nationalitydialog.h"
#include "formvalidatorrules.h"
#include "nationalitydialog.h"
#include "formvalidator.h"
#include "notificator.h"

NationalityDialog::NationalityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NationalityDialog),
    mSelfChangeGuardEnabled(false)
{
    ui->setupUi(this);

    connect(ui->addNationalityButton, &QPushButton::clicked,
            this, &NationalityDialog::addNationalityButtonClicked);

    connect(ui->removeNationalityButton, &QPushButton::clicked,
            this, &NationalityDialog::removeNationalityButtonClicked);

    connect(ui->nationalityTableWidget, &QTableWidget::cellChanged,
            this, &NationalityDialog::nationalityValueUpdated);


    QStringList horizontalLabels;
    horizontalLabels << tr("ID") << tr("Value");
    ui->nationalityTableWidget->setHorizontalHeaderLabels(horizontalLabels);
    ui->nationalityTableWidget->hideColumn(0);

    auto editLineRules = FormValidatorRules::lineEditRules();
    mFormValidator.appendView(ui->nationalityEdit, editLineRules);
    mFormValidator.setAutoFocusEnabled(true);
    mFormValidator.startValidation();

    mNotifier.setParent(this);
    mNotifier.raise();
}

NationalityDialog::~NationalityDialog()
{
    delete ui;
}

void NationalityDialog::setNationalityManager(NationalityManager* m)
{
    mNationalityManager = m;
    auto nationalities = m->nationalities();
    ui->nationalityTableWidget->setRowCount(nationalities.size());

    for (int i = 0; i < nationalities.size(); ++i) {
        auto nationality = nationalities.at(i);
        setNationalityAt(i, nationality);
    }
}

void NationalityDialog::addNationalityButtonClicked()
{
    if (!mFormValidator.validateAll())
        return;

    Nationality nationality(ui->nationalityEdit->text());
    mNationalityManager->appendNationality(nationality);

    if (nationality.id() != -1) {
        mFormValidator.stopValidation();
        ui->nationalityEdit->clear();
        mFormValidator.startValidation();
        int rowCount = ui->nationalityTableWidget->rowCount();
        ui->nationalityTableWidget->insertRow(rowCount);
        setNationalityAt(rowCount, nationality);
    }
}

void NationalityDialog::removeNationalityButtonClicked()
{
    auto selectedIndices = ui->nationalityTableWidget->selectionModel()->selectedRows();
    if (selectedIndices.isEmpty())
        return;

    QVector<int> visualRowsToRemove;

    for (auto selectedIndex : selectedIndices) {
        // Prepare visual indices to remove
        int rowIndex = selectedIndex.row();

        // Remove from database
        auto cell = ui->nationalityTableWidget->item(rowIndex, 0);
        Nationality nationality(cell->text().toInt());

        if (mNationalityManager->isNationalityInUse(nationality)) {
            mNotifier.notify(tr("Can't remove nationality with id '%1' because it is in use")
                             .arg(nationality.id()));
            continue;
        }

        mNationalityManager->removeNationality(nationality);
        visualRowsToRemove << rowIndex;
    }

    std::sort(visualRowsToRemove.begin(), visualRowsToRemove.end(), std::greater<int>());

    for (const auto row: visualRowsToRemove)
        ui->nationalityTableWidget->removeRow(row);
}

void NationalityDialog::nationalityValueUpdated(int row, int column)
{
    if (mSelfChangeGuardEnabled)
        return;

    if (column != 1)
        qDebug() << "Warning changed column index is different from 1";

    int id = ui->nationalityTableWidget->item(row, 0)->text().toInt();
    QString value = ui->nationalityTableWidget->item(row, 1)->text();
    Nationality nationality(id, value);
    mNationalityManager->updateNationality(nationality);
}

void NationalityDialog::resizeEvent(QResizeEvent* event)
{
    mNotifier.resize(event->size());
}

void NationalityDialog::setNationalityAt(int rowIndex, const Nationality& nationality)
{
    mSelfChangeGuardEnabled = true;

    auto idItem = new QTableWidgetItem();
    idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
    idItem->setText(QString::number(nationality.id()));

    auto valueItem = new QTableWidgetItem();
    valueItem->setText(nationality.value());

    ui->nationalityTableWidget->setItem(rowIndex, 0, idItem);
    ui->nationalityTableWidget->setItem(rowIndex, 1, valueItem);

    mSelfChangeGuardEnabled = false;
}
