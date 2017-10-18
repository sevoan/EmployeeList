#ifndef SQLSHORTCUTMODEL_H
#define SQLSHORTCUTMODEL_H

#include <QObject>
#include <QSharedDataPointer>

class SqlShortcutData;

class SqlShortcut
{
public:
    SqlShortcut();
    SqlShortcut(qlonglong id);
    SqlShortcut(const SqlShortcut& other);
    virtual ~SqlShortcut();

    SqlShortcut& operator=(const SqlShortcut& other);
    bool operator==(const SqlShortcut& other) const;

    qlonglong id() const;
    void setId(qlonglong id);

    QString name() const;
    void setName(const QString& value);

    QString sql() const;
    void setSql(const QString& sql);

private:
    QSharedDataPointer<SqlShortcutData> d;
};

class SqlShortcutManager {
public:
    virtual QVector<SqlShortcut> sqlShortcuts() const = 0;
    virtual bool appendSqlShortcut(const SqlShortcut& query) = 0;
    virtual bool removeSqlShortcut(const SqlShortcut& query) = 0;
    virtual bool updateSqlShortcut(const SqlShortcut& query) = 0;

};

#endif // SQLSHORTCUTMODEL_H
