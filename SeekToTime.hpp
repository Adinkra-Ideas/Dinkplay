#ifndef SEEKTOTIME_HPP
#define SEEKTOTIME_HPP

#include <QThread>
#include <QString>
#include <QDebug>

#include "Media.hpp"
#include "SeekToTimeWorker.hpp"

class SeekToTime : virtual public Media
{
    QThread theSeekToTimeThread;

public:
    explicit SeekToTime(QObject *parent = nullptr);
    ~SeekToTime();

    // WE HAVE NOT YET IMPLEMENTED THREAD KILLER
    // Then we can move to seeker proper implementation

    QString     getLengthOfFocusedAudio() override;
    void        updateCurrTimeOfFocusedAudio() override;
    const QString &   getCurrStringTimeOfFocusedAudio() const override;
    const quint32 &   getCurrIntegerTimeOfFocusedAudio() const override;

    QString secondsToDigitalClock(quint32 total);

protected:


    QString digitaltime_;

    QString currTimeOfFocusedAudio_;

private:
    SeekToTimeWorker  seekToTimeWorker_; // Thread for timer
    bool              threadLoopState_;  // false stopped, true running
};

#endif // SEEKTOTIME_HPP
