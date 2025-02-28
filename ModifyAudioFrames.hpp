#ifndef MODIFYAUDIOFRAMES_HPP
#define MODIFYAUDIOFRAMES_HPP

#include "Media.hpp"

class ModifyAudioFrames : virtual public Media
{
public:
    explicit ModifyAudioFrames(QObject *parent = nullptr);
    virtual ~ModifyAudioFrames();

    void generateReversedAudioAtByteLevel(qint16 pathPos) override;
    void generateReversedAudioAtBitLevel(qint16 pathPos) override;

    void extractRawAudioFrames(QString generatedFilePath, QString audioFilePath);
    void encodeAndGenerateModifiedAudioFile(const char* filePath);
    void renameReversedAudio(QString& reversedFileName, const char* prefix);
};

#endif // MODIFYAUDIOFRAMES_HPP
