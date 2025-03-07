#include "ModifyAudioFrames.hpp"

ModifyAudioFrames::ModifyAudioFrames(QObject *parent) :
    Media{parent}
{}

ModifyAudioFrames::~ModifyAudioFrames() {}

void ModifyAudioFrames::renameReversedAudio(QString& reversedFileName, const char* prefix) {
    reversedFileName.insert(reversedFileName.lastIndexOf('/') + 1, prefix);
    reversedFileName.truncate(reversedFileName.lastIndexOf('.'));
    reversedFileName.append(".wav");
}

/**
  * This is called from frontend.
  * to reverse the playing audio sound.
  * @param pathPos the indexof() of the
  * audio sound in audioPaths_
  * @returns void
  */
void ModifyAudioFrames::generateReversedAudioAtByteLevel(qint16 pathPos) {
    // We retrieve the audio filepath if exist.
    QString posToPath = (pathPos >= 0 && pathPos < audioPaths_.size()) ? audioPaths_.at(pathPos) : "";
    if (posToPath.isEmpty()) {
        return ;
    }

    // reversedFileName.prepend("/storage/emulated/0/");

    // WE HAVE TO CHANGE THIS REVERSE FILENAME TO TMP. THEN AT BACKUP POINT, ONLY BACJUP STORAGE/ beginning paths
    // check if Bit-reversed name exists in audioPaths_.
    // dpra_ means Dinkplay Partial Reversed Audio
    QString reversedFileName;

    #ifdef Q_OS_WIN32
    reversedFileName = posToPath;
    renameReversedAudio(reversedFileName, "dpra_");
    #endif
    // on windows, the file can be created in same directory
    // it was copied from without requesting any permission.
    // But for android and iOS, it is easier to just use the
    // tmp directory
    #ifndef Q_OS_WIN32
    // First we retrieve the filename
    reversedFileName = QFileInfo(posToPath).fileName();
    reversedFileName.prepend("dpra_");
    reversedFileName.truncate(reversedFileName.lastIndexOf('.'));
    reversedFileName.append(".wav");
    // WE COMMENTED THIS OUT DURING iOS UPDATE
    // Then we get a temp dirextory
    // QTemporaryDir tempDir;
    // if (tempDir.isValid()) {
    //     reversedFileName = tempDir.path().append("/") + reversedFileName;
    //     tempDir.setAutoRemove(false);
    // } else {
    //     return ;
    // }
    reversedFileName = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + reversedFileName;
    #endif

    if (audioPaths_.contains(reversedFileName)) {
        return ;
    }

    // Extract and store the raw audio frames
    extractRawAudioFrames(reversedFileName, posToPath, false);
}

void ModifyAudioFrames::extractRawAudioFrames(QString generatedFilePath, QString audioFilePath, bool bitLevelReverse) {
    if ( ! audioPaths_.size()
        || soundsHash_.find(*audIt_) == soundsHash_.end() ) {
        return ;
    }
    // Now we ensure the audio that we're trying to copy its audio
    // frames has actually been loaded and decoded by setSource.
    // Also, if the ma_sound_stop() is not first called before the call to
    // ma_data_source_read_pcm_frames(), it will crash the program.
    // Since setSource() calls pause() which in turn calls ma_sound_stop(),
    // we dont have any issue here.
    setSource(audioFilePath.toLocal8Bit().constData());


    // Allocate memory for storing CombinedAudioFrames of audio we're about to read
    // We had to use a void pointer so we can check for null in case malloc fails.
    ma_uint64 byteSizeOfCombinedAudioFrames = device_->playback.channels * (totalPcmFrames_ * ma_get_bytes_per_sample(device_->playback.format)); // size in byte needed to store 1 pcm frame == numberOfOutputChannels * (numberOfPcmFrames * byteSizeOfOnePcmFrame)
    void* vptr = malloc(byteSizeOfCombinedAudioFrames + 1);
    if (vptr == NULL) {
        return ;
    }
    combinedAudioFrames_ = (quint8 *) vptr;

    // WE WILL HOLD ROLLING HERE
    taskRunningDontPlay_ = true;

    // if we pass soundsHash_[QString(*audIt_)]->pDataSource
    // directly to ma_data_source_read_pcm_frames(), it will
    // create a situation where we cannot read a second time
    // from the source without first playing it, else it will
    // return -17 AKA MA_AT_END.
    // To avoid that error, we will make a copy of
    // soundsHash_[QString(*audIt_)] and use this copy instead.
    ma_sound * copiedDataSource;
    copiedDataSource = new ma_sound;
    ma_result result = ma_sound_init_copy(&engine_, soundsHash_[QString(*audIt_)], MA_SOUND_FLAG_DECODE, NULL, copiedDataSource);
    if (result != MA_SUCCESS) {
        delete copiedDataSource;
        free(vptr);
        taskRunningDontPlay_ = false;
        return ;
    }

    // Now we read the audio frames into combinedAudioFrames_
    ma_data_source* dDataSource = copiedDataSource->pDataSource;
    ma_uint64 totalFramesReaded;
    result = ma_data_source_read_pcm_frames(dDataSource, combinedAudioFrames_, totalPcmFrames_, &totalFramesReaded);

    // we can now delete the copy of soundsHash_[QString(*audIt_)]
    ma_sound_uninit(copiedDataSource);
    delete copiedDataSource;

    // If it reaches here, it means we have the audio frames now stored
    // to combinedAudioFrames_. Now we reverse based on partial or full
    // reversal.
    ma_uint64 byteIndexOfLastFrame = device_->playback.channels * (totalFramesReaded * ma_get_bytes_per_sample(device_->playback.format));
    int blockByteSizePerFrame = device_->playback.channels * (1 * ma_get_bytes_per_sample(device_->playback.format));

    // reverse Blocks
    reverseBytesByRange(byteIndexOfLastFrame, blockByteSizePerFrame, bitLevelReverse);

    // Now we encode and store the combinedAudioFrames_
    // to a file using the generatedFilePath as file name
    encodeAndGenerateModifiedAudioFile(generatedFilePath.toLocal8Bit().constData());

    // free memory allocated for holding the raw audio frames
    free(vptr);

    // WE WILL RELEASE ROLLING HERE
    taskRunningDontPlay_ = false;
}

static void copyRange(unsigned char * dest, unsigned char * data, int len) {
    int i = 0;

    while (len > 0) {
        dest[i] = data[i];
        i++;
        len--;
    }
}
void ModifyAudioFrames::reverseBytesByRange(ma_uint64 len, quint8 rangeInBytes, bool bitLevelReverse) {
    if (len == 0 || len < rangeInBytes) {
        return ;
    }

    // Initialize tmp along with l and r cursor points
    unsigned char tmp[128];
    ma_uint64 l = 0;
    ma_uint64 r = len - rangeInBytes;

    while (l < r) {
        // Swap blocks using tmp to temporarily hold the block being swapped
        copyRange(tmp, combinedAudioFrames_ + l, rangeInBytes);
        copyRange(combinedAudioFrames_ + l, combinedAudioFrames_ + r, rangeInBytes);
        copyRange(combinedAudioFrames_ + r, tmp, rangeInBytes);

        // rotate internal block if full reverse
        // THE INTERNAL OF A BLOCK WILL NEVER WORK IF REVERSED.
        // REMEMBER THAT AT 41000 hertz, that's 41,000 Blocks per seconds.
        // Block is the tiniest audio sound that requires thousands to be combined
        // just to make 1 second of sound digitally.
        // if (bitLevelReverse) {
        //     reverseInternalBlock(combinedAudioFrames_ + l, rangeInBytes);
        //     reverseInternalBlock(combinedAudioFrames_ + r, rangeInBytes);
        // }

        // Move pointers towards each other
        l += rangeInBytes;
        r -= rangeInBytes;
    }
    // But there is a bug that we could choose to ignore.
    // rotating a data that has an odd number of blocks like
    // "abcdefghijklmnopqrst"using a rangeInBytes of 4, means we
    // will have "qrstmnopijklefghabcd" which means that the
    // middle "ijkl" block will remain untouched. This is not
    // bug for byte level aka block level reversal, the bug comes
    // when we need to reverse the bytes inside a frame as well.
    // CONCLUSION: IT IS NOT A BUG, SINCE THE INTERNAL OF A BLOCK WILL BECOME
    // CORRUPTED IF ITS BITS GETS REVERSED
}

void ModifyAudioFrames::encodeAndGenerateModifiedAudioFile(const char* filePath) {
    ma_encoder encoder;
    ma_encoder_config config = ma_encoder_config_init(ma_encoding_format_wav, device_->playback.format, device_->playback.channels, device_->sampleRate);
    ma_result createNewAudioFile = ma_encoder_init_file(filePath, &config, &encoder);
    if (createNewAudioFile == MA_SUCCESS) {
        ma_encoder_write_pcm_frames(&encoder, combinedAudioFrames_, totalPcmFrames_, nullptr);
        // if (result = MA_SUCCESS) {
        //     qDebug() << "trr success again. frames written = " << framesWritten;
        // }

        // Add the filepath to Audio List
        indexToAudioList(filePath);
        // make the UI ready to play
        preparePathsForPlay();
    }
    ma_encoder_uninit(&encoder);
}










// unsigned char& reverseBits(unsigned char& b) {
//     b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
//     b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
//     b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
//     return b;
// }
// void reverseInternalBlock(unsigned char* data, unsigned char len) {
//     if (len == 0) {
//         return ;
//     }

//     // Initialize tmp along with l and r cursor points
//     unsigned char tmp;
//     unsigned char l = 0;
//     unsigned char r = len - 1;

//     // Swap characters till l and r meet
//     while (l < r) {
//         // Swap characters
//         tmp = data[l];
//         data[l] = reverseBits(data[r]);
//         data[r] = reverseBits(tmp);

//         // Move pointers towards each other
//         l++;
//         r--;
//     }
// }
// void ModifyAudioFrames::generateReversedAudioAtBitLevel(qint16 pathPos) {
//     // We retrieve the audio filepath if exist.
//     QString posToPath = (pathPos >= 0 && pathPos < audioPaths_.size()) ? audioPaths_.at(pathPos) : "";
//     if (posToPath.isEmpty()) {
//         return ;
//     }

//     // check if Bit-reversed name exists in audioPaths_.
//     // dfra_ means Dinkplay Full Reversed Audio
//     QString reversedFileName = posToPath;
//     renameReversedAudio(reversedFileName, "dfra_");
//     if (audioPaths_.contains(reversedFileName)) {
//         return ;
//     }

//     // Extract and store the raw audio frames
//     extractRawAudioFrames(reversedFileName, posToPath, true);
// }





// 1) A raw audio data simply consists of frames and headers. Forget the headers for now.
// The audio format determines how many bytes a frame carries
// ma_format u8: 1 byte
// ma_format_s16: 2 byte
// ma_format_s24: 3 byte
// ma_format_s32: 4 byte
// ma_format_f32: 4 byte
// 2) A Block is one frame that has a separate copy for each additional channel.
// mono == 1 speaker/channel (1 Block == 1 frame), Stereo == 2 speakers/channels (1 Block == 2 frames) USW.
// if playback rate is z.B. 41000 hertz, it means the audio player is reading out 41000 Blocks per second.
// 3) When reversing frames at the normal ABCD->DCBA Level, it is the Blocks that you're reversing.
// Reversing the frames will only work if it is mono channel(but dont do this). Trying to reverse frames
// when the channel is more than one wont work. You wont even hear shit.
// Now we will try to reverse the block at bit level aka d->b

