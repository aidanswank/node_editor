#include "modules/module.h"
#include "vendor/EasyVst/EasyVst.h"
#include <concurrentqueue/concurrentqueue.h>
#include "midiin_module.h"

struct vst3_midi_module_data
{
    EasyVst vst;
    uint64_t continuousSamples = 0;
//    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
    
    //midi input
    int midi_input_attr;
    midiin_module_data *midi_input_module;
    
    //audio output
    float *audio_output;
};

class vst3_midi_module : public xmodule {
public:
    
    vst3_midi_module()
    {
        xmodule::type = "vst3 module";
    }
    
    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) override;
    void process(std::stack<void *> &value_stack) override;
    void show(const uinode2 &node, example::Graph<Node2> &graph) override;
    
};
