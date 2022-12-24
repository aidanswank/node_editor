#include "vst3_midi_module.h"

void vst3_midi_module::init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes)
{
    uinode2 ui_node;
    ui_node.type = this->type;
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );
    
    vst3_midi_module_data *module_data = new vst3_midi_module_data;
    
    EasyVst vst;
    module_data->vst = vst;
    module_data->audio_output = new float[256]();
    ///Library/Audio/Plug-Ins/VST3/Surge XT.vst3
    /////////////// vst setup TODO CLEAN !!!!
    if (!module_data->vst.init("/Library/Audio/Plug-Ins/VST3/Surge XT.vst3", 44100, 256, Steinberg::Vst::kSample32, true))
    {
        std::cerr << "Failed to initialize VST" << std::endl;
        // return 1;
    }
    
    int numEventInBuses = module_data->vst.numBuses(Steinberg::Vst::kEvent, Steinberg::Vst::kInput);
    int numAudioOutBuses = module_data->vst.numBuses(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
    if (numEventInBuses < 1 || numAudioOutBuses < 1) {
            std::cerr << "Incorrect bus configuration" << std::endl;
            return 1;
    }

    const Steinberg::Vst::BusInfo *outBusInfo = module_data->vst.busInfo(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0);
    if (outBusInfo->channelCount != 2) {
            std::cerr << "Invalid output channel configuration" << std::endl;
            return 1;
    }

    module_data->vst.setBusActive(Steinberg::Vst::kEvent, Steinberg::Vst::kInput, 0, true);
    module_data->vst.setBusActive(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0, true);
    module_data->vst.setProcessing(true);

    
     if (!module_data->vst.createView()) {
           std::cerr << "Failed to create VST view" << std::endl;
           return 1;
     }

    //push module struct as ui node
    push_value_node(module_data, ui_node, graph);
    
    module_data->midi_input_attr = push_value_node(module_data->midi_input_module, ui_node, graph);
//    lop_dat->cutoff_input_attr = push_value_node(lop_dat->cutoff_input, ui_node, graph);
//
    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }
    ui_nodes.push_back(ui_node);
//
    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
}

void vst3_midi_module::process(std::stack<void *> &value_stack)
{
    int SAMPLE_RATE = 44100;
    int TEMPO = 120;
    
    midiin_module_data *midi_input_module = (midiin_module_data *)value_stack.top();
    value_stack.pop();

    vst3_midi_module_data *module_data = (vst3_midi_module_data*)value_stack.top();
        value_stack.pop();
    
    double currentBeat = module_data->continuousSamples / ((60.0 / TEMPO) * static_cast<double>(SAMPLE_RATE));

    Steinberg::Vst::ProcessContext *processContext = module_data->vst.processContext();
    processContext->state = Steinberg::Vst::ProcessContext::kPlaying;
    processContext->sampleRate = SAMPLE_RATE;
    processContext->projectTimeSamples = module_data->continuousSamples;
    processContext->state |= Steinberg::Vst::ProcessContext::kTempoValid;
    processContext->tempo = TEMPO;
    processContext->state |= Steinberg::Vst::ProcessContext::kTimeSigValid;
    processContext->timeSigNumerator = 4;
    processContext->timeSigDenominator = 4;
    processContext->state |= Steinberg::Vst::ProcessContext::kContTimeValid;
    processContext->continousTimeSamples = module_data->continuousSamples;
    processContext->state |= Steinberg::Vst::ProcessContext::kSystemTimeValid;
    processContext->systemTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    processContext->state |= Steinberg::Vst::ProcessContext::kProjectTimeMusicValid;
    processContext->projectTimeMusic = currentBeat;
    module_data->continuousSamples += 256;
        
    Steinberg::Vst::EventList *eventList = module_data->vst.eventList(Steinberg::Vst::kInput, 0);
    while (true) {
        MidiNoteMessage note;
        bool hasNotes = midi_input_module->notesQueue.try_dequeue(note);
        if (!hasNotes) {
                break;
        }
        
        Steinberg::Vst::Event evt = {};
        evt.busIndex = 0;
        evt.sampleOffset = 0;
        evt.ppqPosition = currentBeat;
        evt.flags = Steinberg::Vst::Event::EventFlags::kIsLive;
        if (note.isNoteOn) {
            print("note on");
            evt.type = Steinberg::Vst::Event::EventTypes::kNoteOnEvent;
            evt.noteOn.channel = 0;
            evt.noteOn.pitch = note.noteNum;
            evt.noteOn.tuning = 0.0f;
            evt.noteOn.velocity = note.velocity;
            evt.noteOn.length = 0;
            evt.noteOn.noteId = -1;
        } else {
            print("note off");
            evt.type = Steinberg::Vst::Event::EventTypes::kNoteOffEvent;
            evt.noteOff.channel = 0;
            evt.noteOff.pitch = note.noteNum;
            evt.noteOff.tuning = 0.0f;
            evt.noteOff.velocity = note.velocity;
            evt.noteOff.noteId = -1;
        }
        eventList->addEvent(evt);
    }
    
    if (!module_data->vst.process(256)) {
            std::cerr << "VST process() failed" << std::endl;
            return 1;
    }

    eventList->clear();

    float *left = module_data->vst.channelBuffer32(Steinberg::Vst::kOutput, 0);
    float *right = module_data->vst.channelBuffer32(Steinberg::Vst::kOutput, 1);
    
    float *audio_output = module_data->audio_output;
    
    for (unsigned long i = 0; i < 256; ++i) {
//            outputBuffer[i * 2 + 0] = left[i];
//            outputBuffer[i * 2 + 1] = right[i];
        
        audio_output[i] = left[i];
    }

    value_stack.push(audio_output);
}

void vst3_midi_module::show(const uinode2 &node, example::Graph<Node2> &graph)
{
    vst3_midi_module_data *module_data = (vst3_midi_module_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero

    const float node_width = 100;
    ImNodes::BeginNode(node.id);

    DEBUG_NODE_TITLE_BAR(node.type);

    {
        ImNodes::BeginInputAttribute(module_data->midi_input_attr);
        ImGui::TextUnformatted("midi in");
        ImNodes::EndInputAttribute();
    }

//    {
//        ImNodes::BeginInputAttribute(lop_dat->cutoff_input_attr);
//        ImGui::TextUnformatted("cutoff in");
//        ImNodes::EndInputAttribute();
//    }

//    ImGui::PushItemWidth(node_width);
//    //    if(*jf_data->cutoff_input==0)
//    //    {
//    ImGui::DragFloat("cutoff", &lop_dat->cutoff, 0.01f, 0.f, 1.0f);
//    //    }
//    ImGui::DragFloat("resonance", &lop_dat->resonance, 0.01f, 0.f, 1.0f);
//    ImGui::PopItemWidth();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}
