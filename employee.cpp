#include <QVariant>
#include <QString>
#include <QDebug>

#include "employee_p.h"
#include "employee.h"
#include "weekload.h"

#include <iostream>

QString GenderString(Gender gender)
{
    if (gender == Gender::Male)
        return QObject::tr("Male");
    else if (gender == Gender::Female)
        return QObject::tr("Fermale");

    return QString();
}

QString PositionTypeString(PositionType type)
{
    if (type == PositionType::Primary)
        return QObject::tr("Primary");
    else if (type == PositionType::Secondary)
        return QObject::tr("Secondary");

    return QString();
}

EmployeeData::EmployeeData() :
    id(-1),
    birthYear(0),
    gender(Gender::Male),
    experience(0),
    worksFromYear(0)
{
}

EmployeeData::EmployeeData(const EmployeeData& other) :
    QSharedData(other),
    id(other.id),
    name(other.name),
    birthYear(other.birthYear),
    gender(other.gender),
    nationality(other.nationality),
    positionName(other.positionName),
    education(other.education),
    specialization(other.specialization),
    positionType(other.positionType),
    experience(other.experience),
    worksFromYear(other.worksFromYear),
    teaches(other.teaches),
    studingInAbsentia(other.studingInAbsentia),
    retrainingInformation(other.retrainingInformation),
    conclusions(other.conclusions)
{
}

EmployeeData::~EmployeeData()
{
}

QString EmployeeData::weekLoadSubjectString() const
{
    QString string;

    for (auto load : weekLoad)
        string += load.subject() + ", ";

    return string.left(string.size() - 2);
}

Employee::Employee() :
    d(new EmployeeData())
{
}

Employee::Employee(int id) :
    d(new EmployeeData())
{
    d->id = id;
}

Employee::Employee(const Employee& other) : d(other.d)
{
}

Employee::~Employee()
{
}

Employee&Employee::operator =(const Employee& employee)
{
    d = employee.d;
    return *this;
}

bool Employee::operator==(const Employee& employee) const
{
    return d->id == employee.d->id;
}

int Employee::id() const
{
    return d->id;
}

void Employee::setId(int id)
{
    d->id = id;
}

QString Employee::name() const
{
    return d->name;
}

void Employee::setName(const QString& name)
{
    d->name = name;
}

int Employee::birthYear() const
{
    return d->birthYear;
}

void Employee::setBirthYear(int year)
{
    d->birthYear = year;
}

Gender Employee::gender() const
{
    return d->gender;
}

void Employee::setGender(Gender gender)
{
    d->gender = gender;
}

Nationality Employee::nationality() const
{
    return d->nationality;
}

void Employee::setNationality(const Nationality& nationality)
{
    d->nationality = nationality;
}

QString Employee::positionName() const
{
    return d->positionName;
}

void Employee::setPositionName(const QString& name)
{
    d->positionName = name;
}

QString Employee::education() const
{
    return d->education;
}

void Employee::setEducation(const QString& education)
{
    d->education = education;
}

QString Employee::specialization() const
{
    return d->specialization;
}

void Employee::setSpecialization(const QString& specialization)
{
    d->specialization = specialization;
}

PositionType Employee::positionType() const
{
    return d->positionType;
}

void Employee::setPositionType(PositionType type)
{
    d->positionType = type;
}

int Employee::experience() const
{
    return d->experience;
}

void Employee::setExperience(int experience)
{
    d->experience = experience;
}

int Employee::worksFromYear() const
{
    return d->worksFromYear;
}

void Employee::setWorksFromYear(int year)
{
    d->worksFromYear = year;
}

QVector<WeekLoad> Employee::weekLoads() const
{
    return d->weekLoad;
}

void Employee::setWeekLoads(QVector<WeekLoad> load)
{
    d->weekLoad = load;
}

QVariant Employee::valueAt(int column) const
{
    switch (column) {
        case ID:                    return d->id;
        case NAME:                  return d->name;
        case BIRTH_YEAR:            return d->birthYear;
        case GENDER:                return genderString();
        case NATIONALITY:           return d->nationality.value();
        case POSITION_NAME:         return d->positionName;
        case EDUCATION:             return d->education;
        case SPECIALIZATION:        return d->specialization;
        case POSITION_TYPE:         return positionTypeString();
        case EXPERIENCE:            return d->experience;
        case WORKS_FROM_YEAR:       return d->worksFromYear;
        case TEACHES:               return d->teaches;
        case STUDING_IN_ABSENTIA:   return d->studingInAbsentia;
        case RETRAINING_INFO:       return d->retrainingInformation;
        case CONCLUSIONS:           return d->conclusions;
        case WEEK_LOADS:            return d->weekLoadSubjectString();
        default:                    return QVariant();
    }
}

QString Employee::teaches() const
{
    return d->teaches;
}

void Employee::setTeaches(const QString& value)
{
    d->teaches = value;
}

QString Employee::studingInAbsentia() const
{
    return d->studingInAbsentia;
}

void Employee::setStudingInAbsentia(const QString& value)
{
    d->studingInAbsentia = value;
}

QString Employee::retrainingInformation() const
{
    return d->retrainingInformation;
}

void Employee::setRetrainingInfo(const QString& value)
{
    d->retrainingInformation = value;
}

QString Employee::conclusions() const
{
    return d->conclusions;
}

void Employee::setConclusions(const QString& value)
{
    d->conclusions = value;
}

QString Employee::genderString() const
{
    return GenderString(d->gender);
}

QString Employee::positionTypeString() const
{
    return PositionTypeString(d->positionType);
}
