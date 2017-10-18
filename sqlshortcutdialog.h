#ifndef HOTKEYFILTERSDIALOG_H
#define HOTKEYFILTERSDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>

#include "notificator.h"
#include "formvalidator.h"

namespace Ui {
    class SqlShortcutDialog;
}

class Repository;
class SqlShortcut;
class SqlShortcutManager;

class SqlShortcutDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int testBoxHeight READ testBoxHeight WRITE setTestBoxHeight)

public:
    explicit SqlShortcutDialog(QWidget *parent);
    ~SqlShortcutDialog();

    int testBoxHeight() const;

    void setRepository(Repository* repository);

public Q_SLOTS:
    void saveButtonClicked();
    void editButtonClicked();
    void removeButtonClicked();

    void hideTestsTable();
    void showTestsTable();
    void setTestBoxHeight(int height);

protected:
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void testButtonClicked();
    void resetFields();

private:
    void setSqlShortcutAt(int rowIndex, const SqlShortcut& shortcut);
    void appendSqlShortcut(const SqlShortcut& shortcut);
    SqlShortcut sqlShortcutAt(int rowIndex) const;

    SqlShortcut sqlShortcutFromInputs() const;
    void sqlShortcutToInputs(const SqlShortcut& shortcut);

private:
    Ui::SqlShortcutDialog *ui;
    SqlShortcutManager* mShortcutManager;
    QPropertyAnimation testBoxAnim;
    Notificator mNotificator;
    FormValidator mValidator;
    int mEditingIndex;
};

#endif // HOTKEYFILTERSDIALOG_H
