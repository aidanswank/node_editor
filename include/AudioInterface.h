#include "SDL.h"
#include <vector>
#include <string>

#include "Oscillator.h"
#include "node_editor.h"

class AudioInterface
{
public:
    AudioInterface();
    ~AudioInterface();

    void turnDeviceOn(int device);
    int openDevice(int device, int isCapture);
    void scanAudioDevices();

// private:
    float* float_stream;
    int device_id = 0;
    int buffer_size;
    int sample_rate;

    int num_audio_input_devices;
    int num_audio_output_devices;
    std::vector<std::string> inputDeviceNames;
    std::vector<std::string> outputDeviceNames;

	SDL_AudioSpec spec;

    // testing!!!!
    Oscillator osc;
};