#include "vprint.h"
#include "portaudio.h"
#include <vector>

class audio_interface
{
public:
    audio_interface();
    ~audio_interface();

    void init_devices(int sample_rate, int buffer_size, int input_device_id, int output_device_id);
    virtual void scan_devices();
    void set_param(bool is_capture, int dev_id);
    void turn_on(PaStreamCallback* callback);
    void close_stream();

//private:
    int sample_rate;
    int buffer_size;
    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;
    PaStream *stream;

};
