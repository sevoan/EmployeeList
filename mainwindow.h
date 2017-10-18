#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QPropertyAnimation>

#include "employeetablemodel.h"
#include "notificator.h"

class Repository;
class SqlShortcut;
class EmployeeFilter;
class EmployeeDialog;
class NationalityDialog;
class SqlShortcutDialog;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

public Q_SLOTS:
    void show();

protected:
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:

    void showFiltersDialog();
    void showEmployeeDialog();
    void showNationalityDialog();

    void removeSelectedEmployee();
    void editSelectedEmployee();

    void sqlShortcutAppended(const SqlShortcut& shortcut);
    void sqlShortcutUpdated(const SqlShortcut& newShortcut);
    void sqlShortcutRemoved(const SqlShortcut& shortcut);

    void error(const QString& message);
    void warning(const QString& message);

private:
    int indexOfShortcutButton(const SqlShortcut& shortcut) const;
    void init();

private:
    Ui::MainWindow *ui;

    QScopedPointer<SqlShortcutDialog> mHotkeyFiltersDialog;
    QScopedPointer<NationalityDialog> mNationalityDialog;
    QScopedPointer<EmployeeDialog> mEmployeeDialog;
    QScopedPointer<EmployeeFilter> mEmployeeFilter;
    QScopedPointer<Repository> mRepository;

    Notificator notificator;
};

#endif // MAINWINDOW_H
