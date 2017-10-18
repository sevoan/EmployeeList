#ifndef WEEKLOADPRIVATE_H
#define WEEKLOADPRIVATE_H

#include <QSharedData>

class WeekLoadData : public QSharedData {
public:
    WeekLoadData();
    WeekLoadData(const WeekLoadData& other);
    virtual ~WeekLoadData();

    qlonglong id;
    QString subject;
    int class1_4;
    int class5_9;
    int class10_11;
};

#endif // WEEKLOADPRIVATE_H
