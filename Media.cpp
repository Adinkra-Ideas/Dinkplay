#include "Media.hpp"

/***
 * @brief Media class constructor.
 * @param parent Parent QObject for Qt's object hierarchy.
 *
 * Initializes media player state and audio processing parameters.
 * Sets up iterator for audio paths list.
 */
Media::Media(QObject *parent) :
    QObject{parent},
    appMinimizedStatus_{false},
    repeat_{2},
    state_{PLAYBACK_STOPPED},
    taskRunningDontPlay_{false},
    suspended_{false},
    sampleRate_{0},
    currentFrameNumberToSec_{0},
    totalPcmFrames_{0},
    totalAudioSecs_{0} {
    // set iterator accordingly
    audIt_ = audioPaths_.begin();
}

Media::~Media() {}

// ******************************************
//       GETTERS AND SETTERS BEGINS         *
// ******************************************
/* 0 == repeat none,
 * 1 == repeat 1,
 * 2 == repeat all
 */
void    Media::setRepeat(quint8 val) {
    repeat_ = val;
    emit repeatChanged();
}
quint8  Media::getRepeat() {
    return repeat_;
}



// ******************************************
//       GETTERS AND SETTERS ENDS           *
// ******************************************

/* this method is triggered when Qt::ApplicationState
 * signal is emitted by qt.
 * It then sets the appMinimizedStatus_ value accordingly
 * and emits the appMinimizedStatusChanged() signal afterwards,
 * which will cause UI to refresh by pulling data from Q_PROPERTY
 * calling the Media::getAppMinimizedStatus() method.
*/
void  Media::changeAppLifecycleState(Qt::ApplicationState newState) {
    if (newState == Qt::ApplicationSuspended) {
        appMinimizedStatus_ = true;
        // stop thread running the seek timer
        stopSeekToTimeThreadLoop();
    } else if (newState == Qt::ApplicationActive) {
        appMinimizedStatus_ = false;
        // start thread running the seek timer
        startSeekToTimeThreadLoop();
    }

    // refresh UI
    emit appMinimizedStatusChanged();
}

bool Media::getAppMinimizedStatus() {
    return appMinimizedStatus_;
}
