#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QVector>
#include <QSharedDataPointer>

#include "nationality.h"

enum class Gender {
    Male = 0x7000,
    Female
};

QString GenderString(Gender gender);

enum class PositionType {
    Primary = 0x7100,
    Secondary
};

QString PositionTypeString(PositionType type);

class WeekLoad;
class QVariant;
class EmployeeData;

class Employee
{
public:
    enum Column {
        ID = 0,
        NAME,
        BIRTH_YEAR,
        GENDER,
        NATIONALITY,
        POSITION_NAME,
        EDUCATION,
        SPECIALIZATION,
        POSITION_TYPE,
        EXPERIENCE,
        WORKS_FROM_YEAR,
        TEACHES,
        STUDING_IN_ABSENTIA,
        RETRAINING_INFO,
        CONCLUSIONS,
        WEEK_LOADS,
        COLUMN_COUNT
    };

    Employee();
    Employee(int id);
    Employee(const Employee& other);
    virtual ~Employee();

    Employee& operator =(const Employee& employee);
    bool operator ==(const Employee& employee) const;

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString& name);

    int birthYear() const;
    void setBirthYear(int year);

    Gender gender() const;
    void setGender(Gender gender);

    Nationality nationality() const;
    void setNationality(const Nationality& nationality);

    QString positionName() const;
    void setPositionName(const QString& name);

    QString education() const;
    void setEducation(const QString& education);

    QString specialization() const;
    void setSpecialization(const QString& specialization);

    PositionType positionType() const;
    void setPositionType(PositionType type);

    int experience() const;
    void setExperience(int experience);

    int worksFromYear() const;
    void setWorksFromYear(int year);

    QString teaches() const;
    void setTeaches(const QString& value);

    QString studingInAbsentia() const;
    void setStudingInAbsentia(const QString& value);

    QString retrainingInformation() const;
    void setRetrainingInfo(const QString& value);

    QString conclusions() const;
    void setConclusions(const QString& value);

    QVector<WeekLoad> weekLoads() const;
    void setWeekLoads(QVector<WeekLoad> load);

    QVariant valueAt(int column) const;

    QString genderString() const;
    QString positionTypeString() const;

private:
    QSharedDataPointer<EmployeeData> d;
};

class EmployeeManager {
public:
    virtual QVector<Employee> employees() const = 0;
    virtual void appendEmployee(const Employee& employee) = 0;
    virtual bool removeEmployee(const Employee& employee) = 0;
    virtual bool updateEmployee(const Employee& employee) = 0;
};

#endif // EMPLOYEE_H
