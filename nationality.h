#ifndef NATIONALITY_H
#define NATIONALITY_H

#include <QString>

class Nationality {
public:
    Nationality();
    Nationality(int id);
    Nationality(const QString& value);
    Nationality(int id, const QString &value);

    bool operator ==(const Nationality& nationality);

    int id() const;
    void setId(qlonglong id);

    QString value() const;
    void setValue(const QString& value);

private:
    qlonglong mId;
    QString mValue;
};

class NationalityManager {
public:
    virtual QVector<Nationality> nationalities() const = 0;
    virtual void appendNationality(const Nationality& nationality) = 0;
    virtual void removeNationality(const Nationality& nationality) = 0;
    virtual void updateNationality(const Nationality& nationality) = 0;
    virtual bool isNationalityInUse(const Nationality& nationality) const = 0;
};

#endif // NATIONALITY_H
