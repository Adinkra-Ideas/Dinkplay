#include "SeekToTime.hpp"

SeekToTime::SeekToTime(QObject *parent) :
        Media{parent}
{}

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
  * the signal focusedAudioLengthChanged will be emitted,
  * causing the frontend to refresh by re-pulling the READ
  * method assigned to the length time displayer of
  * MediaControls.qml
  * @param none
  * @returns QString the Audio length converted to
  * digital clock
  */
QString SeekToTime::getFocusedAudioLength() {
    return secondsToDigitalClock(totalAudioSecs_);
}
