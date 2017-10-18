#include "sqlshortcut_p.h"
#include "sqlshortcut.h"

SqlShortcutData::SqlShortcutData()
{
}

SqlShortcutData::SqlShortcutData(const SqlShortcutData& other) :
    QSharedData(other),
    id(other.id),
    name(other.name),
    sql(other.sql)
{
}

SqlShortcutData::~SqlShortcutData()
{
}

SqlShortcut::SqlShortcut() :
    d(new SqlShortcutData())
{
}

SqlShortcut::SqlShortcut(qlonglong id) :
    d(new SqlShortcutData())
{
    d->id = id;
}

SqlShortcut::SqlShortcut(const SqlShortcut& other) :
    d(other.d)
{
}

SqlShortcut::~SqlShortcut()
{
}

SqlShortcut& SqlShortcut::operator=(const SqlShortcut& other)
{
    d = other.d;
    return *this;
}

bool SqlShortcut::operator==(const SqlShortcut& other) const
{
    return d->id == other.d->id;
}

QString SqlShortcut::name() const
{
    return d->name;
}

void SqlShortcut::setName(const QString& name)
{
    d->name = name;
}

QString SqlShortcut::sql() const
{
    return d->sql;
}

void SqlShortcut::setSql(const QString& sql)
{
    d->sql = sql;
}

qlonglong SqlShortcut::id() const
{
    return d->id;
}

void SqlShortcut::setId(qlonglong id)
{
    d->id = id;
}

