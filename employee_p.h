#ifndef EMPLOYEE_DATA_H
#define EMPLOYEE_DATA_H

#include <QObject>
#include <QVector>
#include <QSharedData>

#include "employee.h"

class WeekLoad;

class EmployeeData : public QSharedData {
public:
    EmployeeData();
    EmployeeData(const EmployeeData& other);
    virtual ~EmployeeData();

    QString weekLoadSubjectString() const;

    int id;
    QString name;
    int birthYear;
    Gender gender;
    Nationality nationality;
    QString positionName;
    QString education;
    QString specialization;
    PositionType positionType;
    int experience;
    int worksFromYear;
    QString teaches;
    QString studingInAbsentia;
    QString retrainingInformation;
    QString conclusions;

    QVector<WeekLoad> weekLoad;
};

#endif // EMPLOYEE_DATA_H
