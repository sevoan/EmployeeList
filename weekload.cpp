#include "weekload.h"
#include "weekload_p.h"

WeekLoadData::WeekLoadData()
{
}

WeekLoadData::WeekLoadData(const WeekLoadData& other) :
    QSharedData(other),
    id(other.id),
    subject(other.subject),
    class1_4(other.class1_4),
    class5_9(other.class5_9),
    class10_11(other.class10_11)
{
}

WeekLoadData::~WeekLoadData()
{
}

WeekLoad::WeekLoad() :
    d(new WeekLoadData())
{
}

WeekLoad::WeekLoad(const WeekLoad& other) :
    d(other.d)
{
}

WeekLoad::~WeekLoad()
{
}

WeekLoad&WeekLoad::operator =(const WeekLoad& other)
{
    d = other.d;
    return *this;
}

QString WeekLoad::subject() const
{
    return d->subject;
}

void WeekLoad::setSubject(const QString& value)
{
    d->subject = value;
}

int WeekLoad::class1_4() const
{
    return d->class1_4;
}

void WeekLoad::setClass1_4(int value)
{
    d->class1_4 = value;
}

int WeekLoad::class5_9() const
{
    return d->class5_9;
}

void WeekLoad::setClass5_9(int value)
{
    d->class5_9 = value;
}

int WeekLoad::class10_11() const
{
    return d->class10_11;
}

void WeekLoad::setClass10_11(int value)
{
    d->class10_11 = value;
}
