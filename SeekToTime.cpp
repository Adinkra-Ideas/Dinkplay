#include "SeekToTime.hpp"

SeekToTime::SeekToTime(QObject *parent) :
        Media{parent},
        threadLoopState_{false},
        seekToTimeWorker_{this, threadLoopState_}
{
    connect(this, &Media::startTheSeekToTimeThreadLoop, &seekToTimeWorker_, &SeekToTimeWorker::startTheSeekToTimethreadLoop);
    connect(this, &Media::applicationStateChanged, this, &Media::changeAppLifecycleState);
    connect(&seekToTimeWorker_, &SeekToTimeWorker::oneSecondReached, this, &Media::updateCursorTimeOfFocusedAudio);
    seekToTimeWorker_.moveToThread(&theSeekToTimeThread);
    theSeekToTimeThread.start();

    // This signal will start the blocking method inside the thread
    // emit startTheSeekToTimeThreadLoop(); // commented out to test startup hooking
}

SeekToTime::~SeekToTime() {
    theSeekToTimeThread.quit();
    theSeekToTimeThread.wait();
}

/**
  * Converts the received Seconds
  * to a digital clock for view in
  * Frontend.
  * @param total is the total number
  * of seconds to convert
  * @returns QString the converted
  * digital clock.
  */
const QString SeekToTime::secondsToDigitalClock(quint64 total) const {
    QString digitaltime;

    quint64 minutes = total / 60;
    quint64 seconds = total % 60;
    quint64 hours = minutes / 60;
    minutes = minutes % 60;

    digitaltime = (hours) ? QString::number(hours) + ':' : "";
    digitaltime += (minutes > 9) ? QString::number(minutes) + ':' : QString::number(minutes).prepend('0') + ":";
    digitaltime += (seconds > 9) ? QString::number(seconds) : QString::number(seconds).prepend('0');

    return digitaltime;
}

/**
  * When a new audio is set as source from Player.cpp,
  * the signal lengthofFocusedAudioChanged will be emitted,
  * causing the frontend to refresh by re-pulling the READ
  * method assigned to the length time displayer of
  * MediaControls.qml
  * @param none
  * @returns quint32 the Audio length
  */
const quint64 & SeekToTime::getLengthOfFocusedAudio() const {
    return totalAudioSecs_;
}

const quint64 & SeekToTime::getCursorTimeOfFocusedAudio() const {
    return currentFrameNumberToSec_;
}
// for seeking to time by dragging slider in front view
void  SeekToTime::setCursorTimeOfFocusedAudio(const quint64 & newTime) {
    // we can move to any time in seconds using sampleRate * numberOFseconds
    if (audioPaths_.size() && soundsHash_.find(*audIt_) != soundsHash_.end()) {
        ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], sampleRate_ * newTime);

        // update current time for frontend
        updateCursorTimeOfFocusedAudio();
    }
}
void SeekToTime::updateCursorTimeOfFocusedAudio() {
    currentFrameNumberToSec_ = 0;

    // convert frame currently under cursor to seconds,
    // then pass to secondsToDigitalClock() to get seconds formated to digital clock
    if (audioPaths_.size() && soundsHash_.find(*audIt_) != soundsHash_.end()) {
        // we can read current cursor Point of focused Audio using
        currentFrameNumberToSec_ = ma_sound_get_time_in_pcm_frames(soundsHash_[QString(*audIt_)]) / sampleRate_;
    }

    emit currTimeOfFocusedAudioChanged();
}

void SeekToTime::startSeekToTimeThreadLoop() {
    // Since this method is either called from
    // play() or when the user is bringing the
    // app to front AKA active app.
    // In the later case, we cant just start
    // the thread. Therefore, we check if
    // media is playing first.
    if (state_ == 1) {
        threadLoopState_ = true;
        emit startTheSeekToTimeThreadLoop();
    }
}
void SeekToTime::stopSeekToTimeThreadLoop() {
    // Stop blocking loop in thread
    seekToTimeWorker_.stopTheSeekToTimethreadLoop();
}
