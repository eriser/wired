#include "WiredTest.h"

#include <wx/wx.h>
#include <wx/string.h>

#include "audio/WaveFile.h"

int testWaveFile()
{
    const wxString  stereoWaveFileName("StereoSample.wav");

    WaveFile    wav(stereoWaveFileName);

    TEST_ASSERT(wav.GetNumberOfChannels() == 2);
    TEST_ASSERT(wav.GetNumberOfFrames() == 82880);
    TEST_ASSERT(wav.GetSampleRate() == 44100);
    TEST_ASSERT(wav.GetFormat() == 65538);
    return 0;
}

int main()
{
    testWaveFile();
    return 0;
}
