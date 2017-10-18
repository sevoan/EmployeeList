#ifndef QUERYSHORTCUTDATA_H
#define QUERYSHORTCUTDATA_H

#include <QSharedData>

class SqlShortcutData : public QSharedData {
public:
    SqlShortcutData();
    SqlShortcutData(const SqlShortcutData& other);
    virtual ~SqlShortcutData();

    qlonglong id;
    QString name;
    QString sql;
};

#endif // QUERYSHORTCUTDATA_H
