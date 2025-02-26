#ifndef SEEKTOTIME_HPP
#define SEEKTOTIME_HPP

#include <QThread>
#include <QString>
#include "Media.hpp"
#include "SeekToTimeWorker.hpp"

class SeekToTime : virtual public Media
{
    QThread theSeekToTimeThread;

public:
    explicit SeekToTime(QObject *parent = nullptr);
    ~SeekToTime();

    const quint32 & getLengthOfFocusedAudio() const override;
    void            updateCursorTimeOfFocusedAudio() override;
    const quint32 & getCursorTimeOfFocusedAudio() const override;
    void            setCursorTimeOfFocusedAudio(const quint32 & newTime) override;
    const QString   secondsToDigitalClock(quint32 total) const override;
    void            killSeekToTimeThread() override;

protected:

private:
    SeekToTimeWorker  seekToTimeWorker_; // Thread for timer
    bool              threadLoopState_;  // false stopped, true running
};

#endif // SEEKTOTIME_HPP
