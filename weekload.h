#ifndef WEEKLOAD_H
#define WEEKLOAD_H

#include <QSharedDataPointer>

class WeekLoadData;

class WeekLoad
{
public:
    WeekLoad();
    WeekLoad(const WeekLoad& other);
    virtual ~WeekLoad();

    WeekLoad& operator =(const WeekLoad& other);

    QString subject() const;
    void setSubject(const QString& value);

    int class1_4() const;
    void setClass1_4(int value);

    int class5_9() const;
    void setClass5_9(int value);

    int class10_11() const;
    void setClass10_11(int value);

private:
    QSharedDataPointer<WeekLoadData> d;
};

#endif // WEEKLOAD_H
