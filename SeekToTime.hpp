#ifndef SEEKTOTIME_HPP
#define SEEKTOTIME_HPP

#include <QString>
#include <QDebug>

#include "Media.hpp"

class SeekToTime : virtual public Media
{
public:
    explicit SeekToTime(QObject *parent = nullptr);
    ~SeekToTime();

    QString getFocusedAudioLength() override;

    QString secondsToDigitalClock(quint32 total);

protected:
    QString digitaltime_;
};

#endif // SEEKTOTIME_HPP
