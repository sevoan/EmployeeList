#include "nationality.h"

Nationality::Nationality() : mId(-1)
{
}

Nationality::Nationality(int id) :
    mId(id)
{
}

Nationality::Nationality(const QString& value) :
    mId(-1),
    mValue(value)
{

}

Nationality::Nationality(int id, const QString& value) :
    mId(id),
    mValue(value)
{
}

bool Nationality::operator ==(const Nationality& nationality)
{
    return nationality.mId == mId && nationality.value() == mValue;
}

int Nationality::id() const
{
    return mId;
}

void Nationality::setId(qlonglong id)
{
    mId = id;
}

QString Nationality::value() const
{
    return mValue;
}

void Nationality::setValue(const QString& name)
{
    mValue = name;
}
