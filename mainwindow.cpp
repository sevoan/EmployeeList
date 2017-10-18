#include <QTimer>
#include <QDebug>
#include <QGroupBox>
#include <QLineEdit>
#include <QTableView>
#include <QScrollArea>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QSqlQueryModel>

#include "ui_mainwindow.h"
#include "repository.h"
#include "employee.h"
#include "mainwindow.h"
#include "employeeview.h"
#include "employeefilter.h"
#include "employeedialog.h"
#include "nationalitydialog.h"
#include "sqlshortcutdialog.h"
#include "employeetablemodel.h"

Q_DECLARE_METATYPE(SqlShortcut)

#define PROP_SHORTCUT "sql_shortcut"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mHotkeyFiltersDialog(new SqlShortcutDialog(this)),
    mNationalityDialog(new NationalityDialog(this)),
    mEmployeeDialog(new EmployeeDialog(this)),
    mEmployeeFilter(new EmployeeFilter()),
    mRepository(new Repository())
{
    notificator.setParent(this);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    QWidget::show();
    init();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    notificator.resize(event->size());
}

void MainWindow::error(const QString& message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

void MainWindow::warning(const QString& message)
{
    notificator.notify(message);
}

void MainWindow::showFiltersDialog()
{
    mHotkeyFiltersDialog->show();
}

void MainWindow::showEmployeeDialog()
{
    mEmployeeDialog->showAddEmployee();
}

void MainWindow::showNationalityDialog()
{
    mNationalityDialog->show();
}

void MainWindow::removeSelectedEmployee()
{
    auto selectedRowList = ui->employeeTableView->selectionModel()->selectedRows();
    for (const QModelIndex& selected : selectedRowList) {
        Employee employee(selected.data().toInt());
        mRepository->removeEmployee(employee);
    }
}

void MainWindow::editSelectedEmployee()
{
    auto selectedRowList = ui->employeeTableView->selectionModel()->selectedRows();
    if (selectedRowList.isEmpty())
        return;

    auto model = static_cast<EmployeeTableModel*>(ui->employeeTableView->model());
    Employee employee = model->employeeAt(selectedRowList.first().row());
    mEmployeeDialog->showEditEmployee(employee);
}

void MainWindow::sqlShortcutAppended(const SqlShortcut& shortcut)
{
    QPushButton* button = new QPushButton(shortcut.name());
    button->setProperty(PROP_SHORTCUT, QVariant::fromValue(shortcut));
    button->setCursor(Qt::PointingHandCursor);
    ui->sqlShortcutLayout->addWidget(button);

    connect(button, &QPushButton::clicked, this, [this,button] () {
        auto prop = button->property(PROP_SHORTCUT).value<SqlShortcut>();
        mRepository->filterEmployees(prop.sql());
    });
}

void MainWindow::sqlShortcutUpdated(const SqlShortcut& shortcut)
{
    int buttonIndex = indexOfShortcutButton(shortcut);
    if (buttonIndex == -1)
        return;

    auto item = ui->sqlShortcutLayout->itemAt(buttonIndex);
    auto button = qobject_cast<QPushButton*>(item->widget());
    button->setProperty(PROP_SHORTCUT, QVariant::fromValue(shortcut));
    button->setText(shortcut.name());
}

void MainWindow::sqlShortcutRemoved(const SqlShortcut& shortcut)
{
    int buttonIndex = indexOfShortcutButton(shortcut);
    if (buttonIndex == -1)
        return;

    auto item = ui->sqlShortcutLayout->itemAt(buttonIndex);
    auto widget = item->widget();
    ui->sqlShortcutLayout->removeItem(item);
    widget->deleteLater();
}

int MainWindow::indexOfShortcutButton(const SqlShortcut& shortcut) const
{
    auto childCount = ui->sqlShortcutLayout->count();

    for (int i = 0; i < childCount; ++i) {
        auto item = ui->sqlShortcutLayout->itemAt(i);
        if (item == Q_NULLPTR)
            continue;

        auto button = qobject_cast<QPushButton*>(item->widget());
        if (button == Q_NULLPTR)
            continue;

        auto oldShortcut = button->property(PROP_SHORTCUT).value<SqlShortcut>();
        if (shortcut == oldShortcut)
            return i;
    }

    return -1;
}

void MainWindow::init()
{
    connect(mRepository.data(), &Repository::error, this, &MainWindow::error);
    connect(mRepository.data(), &Repository::warning, this, &MainWindow::warning);

    mRepository->init();

    mHotkeyFiltersDialog->setRepository(mRepository.data());

    EmployeeTableModel* model = new EmployeeTableModel(this);
    model->setEmployeeFilter(mEmployeeFilter.data());
    model->setRepository(mRepository.data());

    ui->employeeTableView->setModel(model);
    ui->employeeTableView->resizeColumnsToContents();
    ui->employeeTableView->setColumnHidden(0, true);

    mNationalityDialog->setNationalityManager(mRepository.data());
    mEmployeeDialog->setNationalityManager(mRepository.data());
    mEmployeeDialog->setEmployeeManager(mRepository.data());

    QPalette palette = ui->employeeTableView->palette();
    palette.setColor(QPalette::Highlight, Qt::transparent);
    ui->employeeTableView->setPalette(palette);

    connect(ui->fastSearchEdit, &QLineEdit::textChanged,
            mEmployeeFilter.data(), &EmployeeFilter::setFilter);

    connect(mRepository.data(), &Repository::sqlShortcutAppended,
            this, &MainWindow::sqlShortcutAppended);

    connect(mRepository.data(), &Repository::sqlShortcutUpdated,
            this, &MainWindow::sqlShortcutUpdated);

    connect(mRepository.data(), &Repository::sqlShortcutRemoved,
            this, &MainWindow::sqlShortcutRemoved);

    // Firstly append reset button
    auto resetButton = new QPushButton(tr("Reset"));
    resetButton->setCursor(Qt::PointingHandCursor);
    ui->sqlShortcutLayout->addWidget(resetButton);

    connect(resetButton, &QPushButton::clicked, this, [this] () {
        mRepository->filterEmployees();
    });

    // Then populate dynamic buttons
    auto shortcuts = mRepository->sqlShortcuts();
    for (auto shortcut : shortcuts)
        sqlShortcutAppended(shortcut);

    // Set notificator at the top of window
    notificator.raise();
}
