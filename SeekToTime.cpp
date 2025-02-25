#include "SeekToTime.hpp"

SeekToTime::SeekToTime(QObject *parent) :
        Media{parent},
        threadLoopState_{true},
        seekToTimeWorker_{this, threadLoopState_}
{
    connect(this, &Media::startTheSeekToTimeThread, &seekToTimeWorker_, &SeekToTimeWorker::startTheSeekToTimethreadLoop);
    connect(&seekToTimeWorker_, &SeekToTimeWorker::oneSecondReached, this, &Media::updateCurrTimeOfFocusedAudio);
    seekToTimeWorker_.moveToThread(&theSeekToTimeThread);
    theSeekToTimeThread.start();

    // This signal will start the blocking method inside the thread
    emit startTheSeekToTimeThread();

}

SeekToTime::~SeekToTime() {}

/**
  * Converts the received Seconds
  * to a digital clock for view in
  * Frontend.
  * @param total is the total number
  * of seconds to convert
  * @returns QString the converted
  * digital clock.
  */
QString SeekToTime::secondsToDigitalClock(quint32 total) {
    quint32 minutes = total / 60;
    quint32 seconds = total % 60;
    quint32 hours = minutes / 60;
    minutes = minutes % 60;

    digitaltime_ = (hours) ? QString::number(hours) + ':' : "";
    digitaltime_ += (minutes > 9) ? QString::number(minutes) + ':' : QString::number(minutes).prepend('0') + ":";
    digitaltime_ += (seconds > 9) ? QString::number(seconds) : QString::number(seconds).prepend('0');

    return digitaltime_;
}

/**
  * When a new audio is set as source from Player.cpp,
  * the signal lengthofFocusedAudioChanged will be emitted,
  * causing the frontend to refresh by re-pulling the READ
  * method assigned to the length time displayer of
  * MediaControls.qml
  * @param none
  * @returns QString the Audio length converted to
  * digital clock
  */
QString SeekToTime::getLengthOfFocusedAudio() {
    return secondsToDigitalClock(totalAudioSecs_);
}


const QString & SeekToTime::getCurrStringTimeOfFocusedAudio() const {
    return currentFrameNumberString_;
}

const quint32 & SeekToTime::getCurrIntegerTimeOfFocusedAudio() const {
    return currentFrameNumberInt_;
}

void SeekToTime::updateCurrTimeOfFocusedAudio() {
    currentFrameNumberInt_ = 0;
    currentFrameNumberString_ = "00:00";

    // convert frame currently under cursor to seconds,
    // then pass to secondsToDigitalClock() to get seconds formated to digital clock
    if (audioPaths_.size() && soundsHash_.find(*audIt_) != soundsHash_.end()) {
        currentFrameNumberInt_ = ma_sound_get_time_in_pcm_frames(soundsHash_[QString(*audIt_)]) / sampleRate_;
        currentFrameNumberString_ = secondsToDigitalClock(currentFrameNumberInt_);
    }
}

// loop emits every sec to now refresh
// setter also emits at end of setting
// refresh current time digital clock in frontend mediacontrols
// emit currTimeOfFocusedAudioChanged();

// QString SeekToTime::setCurrTimeOfFocusedAudio() {
