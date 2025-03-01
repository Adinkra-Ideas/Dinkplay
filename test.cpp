#include <iostream>

unsigned char combinedAudioFrames_[] = "abcdefghijklmnopqrs\0";

void copyRange(unsigned char * dest, unsigned char * data, unsigned short len) {
    unsigned short i = 0;

    while (len > 0) {
        dest[i] = data[i];
        i++;
        len--;
    }
}

void reverseAudioFrames(size_t len, unsigned short blockSize) {
    if (len == 0 || len < blockSize) {
        return ;
    }

    // Initialize tmp along with l and r cursor points
    unsigned char tmp[128];
    size_t l = 0;
    size_t r = len - blockSize;

    while (l < r) {
        // Swap characters using tmp to temporarily hold the swapped data
        copyRange(tmp, combinedAudioFrames_ + l, blockSize);
        copyRange(combinedAudioFrames_ + l, combinedAudioFrames_ + r, blockSize);
        copyRange(combinedAudioFrames_ + r, tmp, blockSize);

        // Move pointers towards each other
        l += blockSize;
        r -= blockSize;
    }
    // But there is a bug that we could choose to ignore.
    // rotating a data that has an odd number of blocks like
    // "abcdefghijklmnopqrst"using a blocksize of 4, means we
    // will have "qrstmnopijklefghabcd" which means that the
    // middle "ijkl" block will remain untouched. This is not
    // bug for byte level aka block level reversal, the bug comes
    // when we need to reverse the bytes inside a frame as well.

}

int main() {
    reverseAudioFrames(19, 4);

    std::cout << combinedAudioFrames_ << std::endl;

    return 0;
}
