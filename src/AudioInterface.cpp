#include "AudioInterface.h"

void myAudioCallback(void *udata, Uint8 *stream, int len)
{
    // printf("?????");
	AudioInterface *audioInterface = (AudioInterface *)udata; // cast context from void pointer
	// Sint16 *str = (Sint16 *)stream;	

    Sint16 *str = (Sint16 *)stream;	

    int faLen = (len / sizeof(Sint16) / 2); // float array len per channel
    float *mystream = new float[faLen*2]();

    float* node_output = new float[faLen*2]();
    node_output = example::NodeEditorAudioCallback();
    // printf("fa %i",faLen);

    audioInterface->osc.setFrequency(440.0);
	for (int i = 0; i < faLen; ++i)
	{
		// // std::cout << mb->tracks[0].stream[0][i] << std::endl;
		// float whitenoise = rand() % 100;
		// whitenoise = whitenoise / 100;
		// // audioInterface->floatStream[i * 2] = mystream[i * 2];
		// // audioInterface->floatStream[i * 2 + 1] = mystream[i * 2 + 1];
		// audioInterface->float_stream[i * 2]     = whitenoise * 0.06;
		// audioInterface->float_stream[i * 2 + 1] = whitenoise * 0.06;

        // double osc_output =  audioInterface->osc.nextSample();
		// audioInterface->float_stream[i * 2]     = osc_output * 0.06;
		// audioInterface->float_stream[i * 2 + 1] = osc_output * 0.06;

		audioInterface->float_stream[i * 2]     = node_output[i]; //MONO for now
		audioInterface->float_stream[i * 2 + 1] = node_output[i];

	}


	for (int i = 0; i < faLen; ++i) // len/size(Sint16)/2 = buffer_size
	{
		str[i * 2]     = (Sint16)(audioInterface->float_stream[i * 2] * 32767);
		str[i * 2 + 1] = (Sint16)(audioInterface->float_stream[i * 2 + 1] * 32767);
	}

}

AudioInterface::AudioInterface()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Couldn't initialize SDL: \n%s\n", SDL_GetError());
        // return 1;
    }

    sample_rate = 44100;
	buffer_size = 256;

	float_stream = new float[buffer_size * 2](); // 2x cuz stereo

	// SDL_AudioSpec spec;
	// TODO: Don't hardcode these values!
	SDL_zero(spec);
	spec.freq = sample_rate;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = buffer_size;
	spec.callback = myAudioCallback;
	spec.userdata = this; // cast from void pointer later in myAudioCallback

    osc.setMode(Oscillator::OSCILLATOR_MODE_SINE);

}

AudioInterface::~AudioInterface()
{
	printf("audio interface shutting down\n");
}

void AudioInterface::turnDeviceOn(int device)
{
    // SDL_PauseAudioDevice(device,0);
	// printf("starting audio device...\n");
}

void AudioInterface::scanAudioDevices()
{
    inputDeviceNames.clear();
    outputDeviceNames.clear();

    num_audio_input_devices = SDL_GetNumAudioDevices(0);
    // print("num_audio_input_devices", num_audio_input_devices);
    for (int i = 0; i < num_audio_input_devices; i++)
    {
        const char *str = SDL_GetAudioDeviceName(i, 1);
        inputDeviceNames.push_back(str);
        printf("%i in device %s\n", i, str);
    };
    // printf("wtf\n");

    num_audio_output_devices = SDL_GetNumAudioDevices(1);
    for (int i = 0; i < num_audio_output_devices; i++)
    {
        const char *str = SDL_GetAudioDeviceName(i, 0);
        outputDeviceNames.push_back(str);
        printf("%i in device %s\n", i, str);
    };

}

int AudioInterface::openDevice(int device, int isCapture)
{
    SDL_AudioSpec obtained;
    SDL_zero(obtained);
    // printf("ajjjjj %s \n",SDL_GetAudioDeviceName(device, 0));

    int old_device_id = device_id;
	device_id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(device, 0), isCapture, &spec, &obtained, 0);
	if (device_id == 0)
	{
		// TODO: Proper error handling
        printf("Couldn't open audio with SDL: \n%s\n", SDL_GetError());
	} else {
        printf("success? %i\n",device_id);
    }
    printf("%i ",obtained.freq);

    // turn off old device when switching
    if(old_device_id!=0)
    {
        SDL_CloseAudioDevice(old_device_id);
    }
    SDL_PauseAudioDevice(device_id,0);

    return device_id;
}