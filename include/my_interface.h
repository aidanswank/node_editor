#include "audio_interface.h"

/*     - setup example -
    my_interface interface;
    interface.scan_devices();                   // check which io devices are available
    interface.init_devices(44100, 1024, 1, 2);  // place device indices here
    interface.open_stream();
*/

struct dev_nfo_wrap
{
    std::string name;
    int id;
};

// place extra data needed by callback in this struct
struct my_interface : public audio_interface
{
    // int extra_data = 123456;
    std::vector<dev_nfo_wrap> input_devices;
    std::vector<dev_nfo_wrap> output_devices;

    void open_stream();
    virtual void scan_devices() override;
};