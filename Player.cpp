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

    playee->endOfCurrentAudio();
}

// **************************************
//          CANINICALS BEGINS           *
// **************************************
Player::Player(QObject *parent) :
    Media{parent},
    engineInit_{false} {


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

    // Also store value of the sample rate for future use
    sampleRate_ = ma_engine_get_sample_rate(&engine_ );
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
        // result_ = ma_sound_init_from_file(&engine_, path, 0, NULL, NULL, soundsHash_[QString(path)]);
        result_ = ma_sound_init_from_file(&engine_, path, MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, soundsHash_[QString(path)]); //loading the MA_SOUND_FLAG_DECODE at all might lead to future issues like suspending the gui when it gets here
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

    readyAudioForNewPlay(); /////
    // set cursor to beginning
    //ma_sound_seek_to_pcm_frame(soundsHash_[QString(path)], 0);
    // connect end call back
    ma_sound_set_end_callback(soundsHash_[QString(path)], my_end_callback, this);

    // Store total length of focused audio in seconds to totalAudioSecs_
    ma_sound_get_length_in_pcm_frames(soundsHash_[QString(*audIt_)], &totalPcmFrames_);
    totalAudioSecs_ = (totalPcmFrames_ / sampleRate_);

    // refresh the UI
    emit playbackStateChanged(currentPlayingPath_);
    // refresh the length of time shown for nowPlaying audio
    emit lengthofFocusedAudioChanged();
    //
    notifyJavaSeviceAboutPlaying(false);
}


/**
  * Path is the sound file intended
  * to be played.
  * I think this is used to play
  * a new sound from the beginning.
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
    if (!engineInit_ || audioPaths_.isEmpty() || taskRunningDontPlay_) {
        return ;
    }

    // Seize control of audio session for both
    // android and iOS
    if (! seizeControlOfAudioSession()) {
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
        //
        startSeekToTimeThreadLoop();
        emit playbackStateChanged(currentPlayingPath_); // 0 stopped, 1 playing, 2 paused
        notifyJavaSeviceAboutPlaying(true);
    }

    // Here, we have successfully copied the raw frames into combinedAudioFrames_.
    // Tomorrow, we will dirst try to play the data from combinedAudioFrames_.
    // Then we will reverse the data in hold the frames

    // // if the ma_sound_stop() is not first called before the call to
    // // ma_data_source_read_pcm_frames(), it will crash the program.
    // pause();
    // ma_data_source* dDataSource = ((ma_sound*) soundsHash_[QString(*audIt_)])->pDataSource;
    // size_t byteSizeOfOnlyRawAudioFrames = device_->playback.channels * (totalPcmFrames_ * ma_get_bytes_per_sample(device_->playback.format)); // size in byte needed to store 1 pcm frame == numberOfOutputChannels * (numberOfPcmFrames * byteSizeOfOnePcmFrame)
    // combinedAudioFrames_ = (char *)malloc(byteSizeOfOnlyRawAudioFrames + 1);
    // ma_uint64 totalFramesReaded;
    // //
    // ma_result resulte = ma_data_source_read_pcm_frames(dDataSource, combinedAudioFrames_, totalPcmFrames_, &totalFramesReaded);
    // qDebug() << "trr byteSizeOfOnlyRawAudioFrames" << byteSizeOfOnlyRawAudioFrames;
    // qDebug() << "trr result== " << resulte;
    // qDebug() << "trr totalFramesReaded == " << totalFramesReaded;
    // qDebug() << "trr total frame count == " << totalPcmFrames_;

    // ma_encoder_config config = ma_encoder_config_init(ma_encoding_format_wav, device_->playback.format, device_->playback.channels, device_->sampleRate);
    // ma_result createNewAudioFile = ma_encoder_init_file("my_file.wav", &config, &encoder);
    // if (createNewAudioFile == MA_SUCCESS) {
    //     qDebug() << "trr success";

    //     ma_uint64 framesWritten;
    //     ma_result storeAudioDataToNewAudioFile = ma_encoder_write_pcm_frames(&encoder, combinedAudioFrames_, totalPcmFrames_, &framesWritten);
    //     if (storeAudioDataToNewAudioFile = MA_SUCCESS) {
    //         qDebug() << "trr success again. frames written = " << framesWritten;
    //     }
    // }
}

// LEARNINGS
// 1) pDataSourceBase->rangeEndInFrames just holds the max value of uint64 so that all
// possible decoded audio will be accommodated. It doesnt return the end in frame.
// It is only passed as param3 to ma_data_source_read_pcm_frames so that
// ma_data_source_read_pcm_frames will stop when it has read the full source from param1
// 2) One frame in mp3 is equivalent to one byte. The reason a raw audio size in byte
// is different from the totalPcmFrames_ is because of the number of output channels. If 2
// channels. then 1 frame will equal 2 bytes. 2 channels == 3 bytes and so on.
// This was tested using mp3. I dont know about wav or the rest formats' byte size per frame.

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
        stopSeekToTimeThreadLoop();
        //
        emit playbackStateChanged(currentPlayingPath_);
        //
        notifyJavaSeviceAboutPlaying(false);
    }

    // This is necessary so mini audio dont lose audio
    // when iOS is returning from an interruption.
    // But we leave it outside if_ios incase android needs it too.
    // HAD TO MAKE THIS ONLY AVAILABLE FOR iOS BECAUSE IN ANDROID, IF YOU DISCONNECT AND THEN RECONNECT BLUETOOTH, SOUND WILL ONLY COME BACK AFTER PLAY->PAUSE->PLAY
    // BUT IF IT IS ABSENT FOR iOS, it causes the play/pause icon in mediainfocenter to go out of sync
    #ifdef Q_OS_IOS
    ma_device_stop(device_);
    #endif
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
        // this will tell endOfCurrentAudio() to stop completely
        // state_ = 0;  // 1 GREAT CHANGE CHNAGED THIS TO COMMENTED OUT
        endOfCurrentAudio(true);     // 2 GREAT CHANGE CHNAGED THIS TO HAVING AN ARG
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
  * This method is auto called whenever
  * current audio is changing to the next
  * or prev, and also whenever there is no
  * repeat set and current audio has to
  * stop completely. In the latter, then
  * this method must be called with
  * endOfCurrentAudio(true).
  * If stopped completely, it emits
  * isStopped because the frontend needs to
  * know. Else it either sets the
  * cursor to beginning and call
  * play() if repeat 1, or it calls
  * next() to move track to next if
  * repeat all.
  * @returns void
  */
void Player::endOfCurrentAudio(bool shouldStopCompletely) { // 4 GREAT CHANGE CHNAGED THIS TO having a param
    if (!engineInit_ || audioPaths_.isEmpty()) {
        return ;
    }

    // THIS STOP LOGIC DOESNT MAKE SENSE
    // BUT FOR NOW WE AR ELOOKING FOR HOW TO CALL UPDATE CC FROM PLAY WHEN PLAY IS BEGINNING OF SOUND
    if (! shouldStopCompletely && repeat_) {
        // verify that there is no next or repeat to do
        if (repeat_ == 1) {
            readyAudioForNewPlay(); /////
            play();
        } else if (repeat_ == 2) {
            changePlay(true);
        }
    }
    else {
        // Here means either endOfCurrentAudio() was called with its shouldStopCompletely
        // parameter set to true OR or current audio reached its end and repeat_ had a
        // value of 0 when the audio ended AKA dont repeat anything.

        readyAudioForNewPlay();

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


// WILL CORRECT THIS LOGIC OF CALLING THIS METHOD FROM QML.
// THIS IS COZ readyAudioForNewPlay() ALREADY STORED DATA
// TO VARIABLES AND QML CAN SIMPLY TAKE DIRECTLY FROM THOSE VARIABLES
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

/////
/**
  * This function prepares the current active audio
  * for new play AKA playing from the beginning.
  * @returns the name.
  */
void Player::readyAudioForNewPlay() {
    // Shift pcm to beginning to signal stop
    ma_sound_seek_to_pcm_frame(soundsHash_[QString(*audIt_)], 0);

    // pause the sound at beginning. Since pause() calls ma_stop,
    // the audio engine will be paused and ios control center will
    // update the play/pause icon automatically because it reads from
    // audio engine.
    pause();

    // current playing state. 0 stopped, 1 playing, 2 paused
    state_ = 0;

    // now we can extract All details of current active audio here
    currentPlayingTitle_ = getTitle();
    currentPlayingArtist_ = "Unknown";
    //MORE REQUIRED DETAILS WILL BE EXTRACTED HERE

    // This plucks the data stored in respective variables at Media class,
    // and ships them to the viewport where needed z.B. control center for iOS
    updateAllAudioDetailsDisplayers();
}
