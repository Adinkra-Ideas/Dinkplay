#include "Media.hpp"

Media::Media(QObject *parent) :
    QObject{parent},
    repeat_{2},
    state_{0},
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

void  Media::changeAppLifecycleState(Qt::ApplicationState newState) {
    if (newState == Qt::ApplicationSuspended) {
        stopSeekToTimeThreadLoop();
    } else if (newState == Qt::ApplicationActive) {
        startSeekToTimeThreadLoop();
    }
}
