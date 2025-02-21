#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QObject>
#include <QDebug>

#include "Media.hpp"

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
extern "C"
{
#include "ma_lib/miniaudio.h"
}
#endif // MINIAUDIO_IMPLEMENTATION

class Player : virtual public Media
{
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    quint8  playbackState() override;
    void    setSource(const char * path) override;
    void    playSource(QString) override;
    void    setPlaybackState(quint8 state) override;
    void    stopAnyCurrentPlaying() override;
    QString getTitle() override;

    void    play() override;
    void    pause() override;
    void    suspendAudio() override;
    void    unsuspendAudio() override;
    void    playOrPause() override;
    void    endOfCurrentAudio(bool shouldStopCompletely = false); // 3 GREAT CHANGE CHNAGED THIS TO having a param
    void    readyAudioForNewPlay();

protected:
    ma_sound            sound_;
    ma_result           result_;
    ma_engine           engine_;
    ma_device         * device_;  // After the ma_engine_init() ints our engine, we backup the device it created for us here so we can use it when necessary
    ma_device_config    config_;

    bool        engineInit_;    // True if mini audio engine was successfully init-ed, else false
    quint8      state_;         // Holds the current media playback state at any given time. 0 == stopped, 1 == playing, 2 == paused

    ma_uint32   sampleRate_;        // sampleRate is the number of PCM frames that are processed per second. Useful for converting PCM frames to seconds or milliseconds since ma_sound_seek_to_pcm_frame() has no seek to seconds alt. To jump to 2 secs just do (sampleRate * 2)
    ma_uint64   totalPcmFrames_;    // totalPcmFrames_ is the total number of PCM frames in focused audio file. Useful for getting the total length of focused audio in seconds with (totalPcmFrames_ / sampleRate_)
    ma_uint32   totalAudioSecs_;    // totalAudioSecs_ == total length of focused audio in seconds
};

#endif // PLAYER_HPP


// FOR SEEKING AKA DRAG TIME FORWARD OR BACKWARD
//     * we can move to any time in seconds using sampleRate * numberOFseconds z.B, ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], sampleRate_ * 25);

// FOR THREAD THAT UPDATES TIME EVERY SECOND IF PLAYING STATE IS TRUE
//     * we can read current Point of focused Audio using
//     ma_sound_get_time_in_pcm_frames(soundsHash_[QString(*audIt_)]);

