#include <QDebug>
#include <QSqlError>
#include <QLineEdit>
#include <QSqlQuery>
#include <QResizeEvent>
#include <QTableWidget>
#include <QSqlTableModel>

#include "ui_sqlshortcutdialog.h"
#include "sqlshortcutdialog.h"
#include "formvalidatorrules.h"
#include "repository.h"

#define ANIM_DURATION           150
#define TEST_TABLE_MAX_HEIGHT   200

SqlShortcutDialog::SqlShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SqlShortcutDialog),
    mShortcutManager(Q_NULLPTR),
    mEditingIndex(-1)
{
    ui->setupUi(this);
    ui->testGroupBox->setMaximumHeight(0);
    ui->shortcutNameEdit->setFocus();

    // Hide id column
    ui->shortcutsTable->setColumnHidden(0, true);

    connect(this, &SqlShortcutDialog::rejected, this, &SqlShortcutDialog::resetFields);
    connect(this, &SqlShortcutDialog::accepted, this, &SqlShortcutDialog::resetFields);

    testBoxAnim.setTargetObject(this);
    testBoxAnim.setPropertyName("testBoxHeight");
    testBoxAnim.setEasingCurve(QEasingCurve::OutCubic);

    ui->testResultTable->setModel(new QSqlQueryModel());

    mNotificator.setParent(this);
    mNotificator.raise();

    auto notEmptyRules = FormValidatorRules::lineEditRules();
    mValidator.appendView(ui->shortcutNameEdit, notEmptyRules);
    mValidator.appendView(ui->shortcutQueryEdit, notEmptyRules);
    mValidator.setAutoFocusEnabled(true);
    mValidator.startValidation();
}

SqlShortcutDialog::~SqlShortcutDialog()
{
    delete ui;
}

void SqlShortcutDialog::hideTestsTable()
{
    int duration = testBoxHeight() * ANIM_DURATION / TEST_TABLE_MAX_HEIGHT;

    testBoxAnim.stop();
    testBoxAnim.setStartValue(testBoxHeight());
    testBoxAnim.setEndValue(0);
    testBoxAnim.setDuration(duration);
    testBoxAnim.start();
}

void SqlShortcutDialog::showTestsTable()
{
    int duration = (TEST_TABLE_MAX_HEIGHT - testBoxHeight()) *
                   ANIM_DURATION / TEST_TABLE_MAX_HEIGHT;

    testBoxAnim.stop();
    testBoxAnim.setStartValue(testBoxHeight());
    testBoxAnim.setEndValue(TEST_TABLE_MAX_HEIGHT);
    testBoxAnim.setDuration(duration);
    testBoxAnim.start();
}

int SqlShortcutDialog::testBoxHeight() const
{
    return ui->testGroupBox->maximumHeight();
}

void SqlShortcutDialog::setRepository(Repository* repository)
{
    if (repository == Q_NULLPTR)
        return;

    mShortcutManager = repository;

    ui->shortcutsTable->clear();

    auto sqlShortcuts = repository->sqlShortcuts();
    for (auto sqlShortcut : sqlShortcuts)
        appendSqlShortcut(sqlShortcut);
}

void SqlShortcutDialog::testButtonClicked()
{
    if (mShortcutManager == Q_NULLPTR || !mValidator.validate(ui->shortcutQueryEdit))
        return;

    QString text = ui->shortcutQueryEdit->text();

    // Hack: Arising transformation
    auto query = static_cast<Repository*>(mShortcutManager)->rawQuery(text);
    if (!query.exec()) {
        qDebug() << query.lastError();
        mNotificator.notify(tr("Error: %1").arg(query.lastError().text()));
        return;
    }

    auto model = static_cast<QSqlQueryModel*>(ui->testResultTable->model());
    model->setQuery(query);

    showTestsTable();
}

void SqlShortcutDialog::saveButtonClicked()
{
    if (!mValidator.validateAll())
        return;

    bool isEditing = mEditingIndex != -1;
    auto shortcut = sqlShortcutFromInputs();
    bool done = isEditing ? mShortcutManager->updateSqlShortcut(shortcut) :
                            mShortcutManager->appendSqlShortcut(shortcut);
    if (done) {
        if (isEditing)
            setSqlShortcutAt(mEditingIndex, shortcut);
        else
            appendSqlShortcut(shortcut);

        resetFields();
    }
}

void SqlShortcutDialog::editButtonClicked()
{
    auto selectedIndices = ui->shortcutsTable->selectionModel()->selectedRows();
    if (selectedIndices.isEmpty())
        return;

    mEditingIndex = selectedIndices.first().row();
    auto sqlShortcut = sqlShortcutAt(mEditingIndex);
    sqlShortcutToInputs(sqlShortcut);
}

void SqlShortcutDialog::removeButtonClicked()
{
    auto selectedIndices = ui->shortcutsTable->selectionModel()->selectedRows();
    if (selectedIndices.isEmpty())
        return;

    QVector<int> visualRowsToRemove;

    for (auto selectedIndex : selectedIndices) {
        // Prepare visual indices to remove
        int rowIndex = selectedIndex.row();

        // Remove from database
        auto idItem = ui->shortcutsTable->item(rowIndex, 0);

        SqlShortcut sqlShortcut(idItem->text().toLongLong());
        if (mShortcutManager->removeSqlShortcut(sqlShortcut))
            visualRowsToRemove << rowIndex;
    }

    std::sort(visualRowsToRemove.begin(), visualRowsToRemove.end(), std::greater<int>());

    for (auto row: visualRowsToRemove)
        ui->shortcutsTable->removeRow(row);
}

void SqlShortcutDialog::setTestBoxHeight(int height)
{
    ui->testGroupBox->setMaximumHeight(height);
}

void SqlShortcutDialog::resizeEvent(QResizeEvent* event)
{
    mNotificator.resize(event->size());
}

void SqlShortcutDialog::resetFields()
{
    mEditingIndex = -1;

    mValidator.stopValidation();

    ui->shortcutIdEdit->clear();
    ui->shortcutNameEdit->clear();
    ui->shortcutQueryEdit->clear();

    mValidator.startValidation();
}

void SqlShortcutDialog::setSqlShortcutAt(int rowIndex, const SqlShortcut& shortcut)
{
    auto idItem     = ui->shortcutsTable->item(rowIndex, 0);
    auto nameItem   = ui->shortcutsTable->item(rowIndex, 1);
    auto sqlItem    = ui->shortcutsTable->item(rowIndex, 2);

    idItem->setText(QString::number(shortcut.id()));
    nameItem->setText(shortcut.name());
    sqlItem->setText(shortcut.sql());
}

void SqlShortcutDialog::appendSqlShortcut(const SqlShortcut& shortcut)
{
    int rowIndex = ui->shortcutsTable->rowCount();
    ui->shortcutsTable->insertRow(rowIndex);

    auto idItem = new QTableWidgetItem();
    idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
    idItem->setText(QString::number(shortcut.id()));

    auto nameItem = new QTableWidgetItem();
    nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
    nameItem->setText(shortcut.name());

    auto sqlItem = new QTableWidgetItem();
    sqlItem->setFlags(sqlItem->flags() ^ Qt::ItemIsEditable);
    sqlItem->setText(shortcut.sql());


    ui->shortcutsTable->setItem(rowIndex, 0, idItem);
    ui->shortcutsTable->setItem(rowIndex, 1, nameItem);
    ui->shortcutsTable->setItem(rowIndex, 2, sqlItem);
}

SqlShortcut SqlShortcutDialog::sqlShortcutAt(int rowIndex) const
{
    auto idItem     = ui->shortcutsTable->item(rowIndex, 0);
    auto nameItem   = ui->shortcutsTable->item(rowIndex, 1);
    auto sqlItem    = ui->shortcutsTable->item(rowIndex, 2);

    SqlShortcut shortcut(idItem->text().toLongLong());
    shortcut.setName(nameItem->text());
    shortcut.setSql(sqlItem->text());

    return shortcut;
}

SqlShortcut SqlShortcutDialog::sqlShortcutFromInputs() const
{
    SqlShortcut shortcut(ui->shortcutIdEdit->text().toLongLong());
    shortcut.setName(ui->shortcutNameEdit->text());
    shortcut.setSql(ui->shortcutQueryEdit->text());
    return shortcut;
}

void SqlShortcutDialog::sqlShortcutToInputs(const SqlShortcut& shortcut)
{
    ui->shortcutIdEdit->setText(QString::number(shortcut.id()));
    ui->shortcutNameEdit->setText(shortcut.name());
    ui->shortcutQueryEdit->setText(shortcut.sql());
}
