#include "my_interface.h"
#include "node_editor.h"

float* node_output2 = new float[512*2]();
// write custom callback below
// i made it static because its not supposed to be called outside this file by that name
static int custom_callback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    // print("hello??");
    /* Cast data passed through stream to our structure. */
    my_interface *interface = (my_interface*)userData;
    // print("extra data",interface->extra_data);

    float *out = (float*)outputBuffer;
    unsigned int i;
    // (void) inputBuffer; /* Prevent unused variable warning. */
    float* audio_in = (float*)inputBuffer;
    
    node_output2 = example::NodeEditorAudioCallback();

    for( i=0; i<framesPerBuffer; i++ )
    {
        out[i * 2] = node_output2[i];  /* left */
        out[i * 2 + 1] = node_output2[i];  /* right */
    }
    return 0;
}

// connect callback and struct
void my_interface::open_stream()
{
    turn_on(custom_callback);
}

void my_interface::scan_devices()
{
    input_devices.clear();
    output_devices.clear();

    int numDevices;
    numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
    }

    const PaDeviceInfo *deviceInfo;
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
        print(i,"~",deviceInfo->name,"\ndefault sample rate:", deviceInfo->defaultSampleRate,"max inputs:",deviceInfo->maxInputChannels,"max outputs:",deviceInfo->maxOutputChannels,"\n");

//        dev_nfo_wrap nfo { deviceInfo->name, i };
        device_id_map.insert( {deviceInfo->name, i} );

        if(deviceInfo->maxInputChannels>0)
        {
            input_devices.push_back(deviceInfo->name);
        }
        if(deviceInfo->maxOutputChannels>0)
        {
            output_devices.push_back(deviceInfo->name);
        }
    }

//    for(int i = 0; i < input_devices.size(); i++)
//    {
//        print("IN  ~ id",input_devices[i].id,"name",input_devices[i].name);
//    }
//    for(int i = 0; i < output_devices.size(); i++)
//    {
//        print("OUT ~ id",output_devices[i].id,"name",output_devices[i].name);
//    }
}
