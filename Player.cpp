#include "Player.hpp"

/**
  * This is a callback function passed
  * to miniaudio player for it to exec
  * when active sound reaches endOfPlay.
  * @param pUserData is a pointer to
  * this class, while param pSound is
  * a pointer to the sound var
  * @returns void
  */
static void my_end_callback(void* pUserData, ma_sound* pSound)
{
    Player * playee = (Player *) pUserData;

    if (playee == nullptr) {
        return;
    }

    playee->stop();
}

// **************************************
//          CANINICALS BEGINS           *
// **************************************
Player::Player(QObject *parent) :
    Media{parent},
    engineInit_{false},
    state_{0} {


    // initialize the mini audio engine object.
    // will be destroyed in destructor
    result_ = ma_engine_init(NULL, &engine_);
    if (result_ != MA_SUCCESS) {
        return ;
    }
    engineInit_ = true;

    // It is necessary to back this created device
    // up so we can use ma_device_start() and
    // ma_device_stop() when necessary.
    device_ = ma_engine_get_device(&engine_);
}

Player::~Player() {
    // // Free sound_ memory
    // ma_sound_uninit(&sound_);
    // free sounds stored to map
    for(auto & oneKey : soundsHash_) {
        if (soundsHash_[oneKey.first] != nullptr) {
            ma_sound_uninit(soundsHash_[oneKey.first]);
            delete soundsHash_[oneKey.first];
        }
    }
    // Free engine_ memory on closing the music player
    ma_engine_uninit(&engine_);
}
// **************************************
//          CANONICALS ENDS             *
// **************************************


/**
  * Sets the received path string
  * as source for active media.
  * Then sets cursor to beginning
  * and emits playIsStopped because
  * Frontend needs to know.
  * @param path is the filepath of
  * the new sound that should be
  * be set as current Active media
  * @returns void
  */
void Player::setSource(const char * path) {
    if (!engineInit_) {
        return ;
    }

    // if there is an active, we stop the ma_sound.
    // THIS MUST BE DONE BEFORE TAMPERING WITH
    // audIt_ or *audIt_ or audioPaths_
    stopAnyCurrentPlaying();

    // check if qstringslist contains this filepath.
    qsizetype pos = audioPaths_.indexOf(path);
    if (pos == -1) {
        return ;
    }
    // re-calibrate the qstringslist iterator
    audIt_ = audioPaths_.begin() + pos;

    // set the current playing audio path to currentPlayingPath_
    currentPlayingPath_ = path;

    // The idea is: although the soundsHash_ contains
    // all the mp3 files indexed from addDirectory(),
    // the ma_sound associated with a mp3 file will only
    // become initialized during the first time the user
    // requested to play that track.
    // And also, memory will only be freed for all the
    // paths at destructor, when the app is closed.
    if (soundsHash_[QString(path)] == nullptr) {
        soundsHash_[QString(path)] = new ma_sound;
        result_ = ma_sound_init_from_file(&engine_, path, 0, NULL, NULL, soundsHash_[QString(path)]);
        if (result_ == MA_SUCCESS) {
            // calls itself again this time with a sound to play
            setSource(path);
            return ;
        } else {
            delete soundsHash_[QString(path)];
            soundsHash_[QString(path)] = nullptr;
            return ;
        }
    }

    // set cursor to beginning
    ma_sound_seek_to_pcm_frame(soundsHash_[QString(path)], 0);
    // connect end call back
    ma_sound_set_end_callback(soundsHash_[QString(path)], my_end_callback, this);
    // set current playing state to "stopped"
    state_ = 0;
    // refresh the UI
    emit playbackStateChanged(currentPlayingPath_);
    //
    notifyJavaSeviceAboutPlaying(false);
}

/**
  * Path is the sound file intended
  * to be played.
  * @returns void
  */
void Player::playSource(QString path) {
    if (!engineInit_) {
        return ;
    }

    setSource(path.toLocal8Bit().constData());
    play();
}

/**
  * Simply plays the active sound
  * in sound_, and emit isplaying
  * because Frontend needs to know.
  * @returns void
  */
void Player::play() {
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }

    // This is necessary so mini audio dont lose audio
    // when iOS is returning from an interruption
    ma_device_start(device_);

    // play the sound
    ma_sound_start(soundsHash_[QString(*audIt_)]);
    // emit playing only if sounds started playing
    if (ma_sound_is_playing(soundsHash_[QString(*audIt_)])) {
        state_ = 1;
        emit playbackStateChanged(currentPlayingPath_); // 0 stopped, 1 playing, 2 paused
        notifyJavaSeviceAboutPlaying(true);
    }
}

/**
  * Simply pauses the active sound
  * in sound_, and emit isPaused
  * because Frontend needs to know.
  * @returns void
  */
void Player::pause() {
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }
    // pause the sound
    ma_sound_stop(soundsHash_[QString(*audIt_)]);
    // emit paused only if sounds paused
    if (!ma_sound_is_playing(soundsHash_[QString(*audIt_)])) {
        // 0 stopped, 1 playing, 2 paused
        state_ = 2;
        //
        emit playbackStateChanged(currentPlayingPath_);
        //
        notifyJavaSeviceAboutPlaying(false);
    }

    // This is necessary so mini audio dont lose audio
    // when iOS is returning from an interruption.
    // But we leave it outside if_ios incase android needs it too.
    ma_device_stop(device_);
}

/**
  * simply sets the suspended_ variable
  * to true after calling pause().
  * Setting the suspended_ variable to
  * true means if unsuspendAudio() is
  * called afterwards, the audio will
  * resume playing.
  * @returns void
  */
void Player::suspendAudio() {
    if (ma_sound_is_playing(soundsHash_[QString(*audIt_)])) {
        pause();
        suspended_ = true;
    }
}

/**
  * Only calls play() if suspended_ is true.
  * This is because if suspended_ == true, it
  * means the currently playing was suspended
  * temporarily waiting to be resumed maybe due
  * to an audio interruption or ducking,
  * TIP: we wont want to resume if it was an
  * audio ducking though. Learn from VLC
  * @returns void
  */
void Player::unsuspendAudio() {
    if (suspended_) {
        play();
        suspended_ = false;
    }
}

/**
  * Simply pauses the active sound
  * if playig or plays if paused.
  * because Frontend needs to know.
  * @returns void
  */
void Player::playOrPause() {
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }

    if (ma_sound_is_playing(soundsHash_[QString(*audIt_)])) {
        pause();
    } else {
        play();
    }
}

/**
  * Simply providing a medium through
  * which the frontend can set stop,
  * pause and play to sound
  * @returns void
  */
void Player::setPlaybackState(quint8 state) {
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }

    if (state == 0) {
        // this will tells stop() to stop()
        // the active sound regardless of anything.
        state_ = 0;
        stop();
    } else if (state == 1) {
        play();
    } else if (state == 2) {
        pause();
    }
}

/**
  * We are checking if there is an active
    // index already locked to the player. If
    // true, we stop the ma_sound in the active
    // index whether it is currently playing or not.
    // This will only be false when the directory
    // is still empty as at the time when the use
    // clicked to select a folder for indexing.
  * @returns the current sound_ play status.
  */
void    Player::stopAnyCurrentPlaying() {
    if (!audioPaths_.isEmpty()
        && soundsHash_.count(*audIt_) > 0
        && soundsHash_[QString(*audIt_)] != nullptr
        ) {
        // stop any currently playing if any
        setPlaybackState(0);
    }
}

/**
  * Stops the sound BUT only if there
  * is no repeat_one or repeat_all set.
  * If you want the sound to be stopped
  * regardless of the above, then you
  * have to call setPlaybackState(0)
  * instead.
  * If stopped, it emits isStopped
  * because the frontend needs to
  * know. Else it either sets the
  * cursor to beginning and call
  * play() if repeat 1, or it calls
  * next() to move track to next if
  * repeat all.
  * @returns void
  */
void Player::stop() {
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }

    if (state_ == 0) {
        // Here means a direct instruction was sent from
        // setPlaybackState() to set the player to stop.
        // THIS CONDITION MUST BE AT THE TOP!

        // Shift pcm to beginning to signal stop
        // Shift pcm to beginning to signal stop
        ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], 0);
        // pause the sound at beginning
        ma_sound_stop(soundsHash_[QString(*audIt_)]);
    }
    // verify that there is no next or repeat to do
    else if (repeat_ == 1) {
        // set cursor to beginning
        ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], 0);
        // pause the sound at beginning
        ma_sound_stop(soundsHash_[QString(*audIt_)]);
        // call play()
        play();
    } else if (repeat_ == 2) {
        changePlay(true);
    } else {
        // Here means it reached a final stop.
        // Shift pcm to beginning to signal stop
        ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], 0);
        // pause the sound at beginning
        ma_sound_stop(soundsHash_[QString(*audIt_)]);
        // set state to "stopped"
        state_ = 0;
        // Refresh UI
        emit playbackStateChanged(currentPlayingPath_);   // 0 stopped, 1 playing, 2 paused
        //
        notifyJavaSeviceAboutPlaying(false);
    }
}

/**
  * For getting the current sound_ playing
  * status.
  * @returns the current sound_ play status.
  */
quint8  Player::playbackState() {
    return state_;
}


/**
  * For getting the name of current sound_
  * @returns the name.
  */
QString Player::getTitle() {
    if (!audioPaths_.isEmpty()
        && soundsHash_.count(*audIt_) > 0
        && soundsHash_[QString(*audIt_)] != nullptr
        ) {
        qsizetype pos = (*audIt_).lastIndexOf("/", -1);
        return (*audIt_).sliced(pos + 1);
    } else {
        return "";
    }
}



