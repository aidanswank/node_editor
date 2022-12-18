#include "adsr_module.h"

void adsr_module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes, std::string module_name)
{
    uinode2 ui_node;
    ui_node.type = module_name;
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );

    EnvelopeGenerator *envelope_generator = new EnvelopeGenerator;
    envelope_generator->setSampleRate(44100);
    
    // set up module struct
    adsr_data *module_data = new adsr_data
    {
        new float[256](),
        NULL,
        envelope_generator
    };
    
//    module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_ATTACK] = 0.123;

    module_data->attack = module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_ATTACK];
    module_data->decay = module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_DECAY];
    module_data->sustain = module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN];
    module_data->release = module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_RELEASE];

    //push module struct as ui node
    push_value_node(module_data, ui_node, graph);

    // set up io of struct
//    module_data->input_audio_attr = push_value_node(module_data->input_audio, ui_node, graph);

    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }

    ui_nodes.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
}

void adsr_module_process(std::stack<void *> &value_stack){
    // pop off audio input data from other modules
    adsr_data *module_data = (adsr_data*)value_stack.top();
    value_stack.pop();
    
    EnvelopeGenerator *envelope_generator = module_data->envelope_generator;
    
    
    module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_ATTACK] = module_data->attack;
    module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_DECAY] = module_data->decay;
    module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN] = module_data->sustain;
    module_data->envelope_generator->stageValue[EnvelopeGenerator::ENVELOPE_STAGE_RELEASE] = module_data->release;

    for(int i = 0; i < 256; i++)
    {
        double envelope = envelope_generator->nextSample();
//        print(envelope);
        module_data->envelope_signal[i] = envelope;
    }

    value_stack.push(module_data->envelope_signal);

}

void adsr_module_show(const uinode2 &node, example::Graph<Node2> &graph){
    //             freevoice->envelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
    adsr_data* module_data = (adsr_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
    
    const float node_width = 100;
    ImNodes::BeginNode(node.id);

    DEBUG_NODE_TITLE_BAR(node.type);
    
    if (ImGui::Button("Click me!"))
    {
        // Button was clicked, do something
        module_data->envelope_generator->enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
    }

//    {
//        ImNodes::BeginInputAttribute(adsr_data->input_audio_attr);
//        ImGui::TextUnformatted("input");
//        ImNodes::EndInputAttribute();
//    }

    ImGui::PushItemWidth(node_width);
    ImGui::DragFloat("attack",&module_data->attack, 0.01f, 0.01f, 1.0f);
    ImGui::DragFloat("decay",&module_data->decay, 0.01f, 0.01f, 1.0f);
    ImGui::DragFloat("sustain",&module_data->sustain, 0.01f, 0.f, 1.0f);
    ImGui::DragFloat("release",&module_data->release, 0.01f, 0.f, 1.0f);
//    ImGui::DragFloat("resonance", &jf_data->resonance, 0.01f, 0.f, 1.0f);
    ImGui::PopItemWidth();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
}
