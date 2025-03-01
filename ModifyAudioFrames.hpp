#ifndef MODIFYAUDIOFRAMES_HPP
#define MODIFYAUDIOFRAMES_HPP

#include "Media.hpp"

class ModifyAudioFrames : virtual public Media
{
public:
    explicit ModifyAudioFrames(QObject *parent = nullptr);
    virtual ~ModifyAudioFrames();

    void generateReversedAudioAtByteLevel(qint16 pathPos) override;
    // void generateReversedAudioAtBitLevel(qint16 pathPos) override;

    void extractRawAudioFrames(QString generatedFilePath, QString audioFilePath, bool bitLevelReverse);
    void encodeAndGenerateModifiedAudioFile(const char* filePath);
    void renameReversedAudio(QString& reversedFileName, const char* prefix);
    void reverseBytesByRange(ma_uint64 len, quint8 rangeInBytes, bool bitLevelReverse);
};

#endif // MODIFYAUDIOFRAMES_HPP
