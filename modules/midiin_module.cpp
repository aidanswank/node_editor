#include "midiin_module.h"

void midiin_module::init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes)
{
    
    uinode2 ui_node;
    ui_node.type = this->type;
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );

//    UserData *userdata = new UserData;
//    userdata->continuousSamples = 0;

    midiin_module_data *module_data = new midiin_module_data;
//    module_data->node_id = ui_node.id;
    print("midi in module init");
    RtMidiIn *midiIn = nullptr;
    try
    {
        midiIn = new RtMidiIn();
    }
    catch (RtMidiError &err)
    {
        err.printMessage();
        // return 1;
    }
    
    
    unsigned int nPorts = midiIn->getPortCount();
    if (nPorts == 0)
    {
        std::cout << "No ports available!\n";
    }
    
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
    std::string portName;
    for ( unsigned int i=0; i<nPorts; i++ ) {
        try {
        portName = midiIn->getPortName(i);
        }
        catch ( RtMidiError &error ) {
        error.printMessage();
        }
        module_data->port_names.push_back(portName);
        std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
    }
    
    
//    try
//    {
//        midiIn->openPort(2);
////            midiIn->setCallback(&midiCallback, userdata);
//        midiIn->setCallback(&midiCallback, module_data);
//    }
//    catch (RtMidiError &err)
//    {
//        err.printMessage();
//        // return 1;
//    }
    
    std::vector<MidiNoteMessage> notes;
    
//        module_data->userdata_ptr = userdata;
    module_data->midiin_ptr = midiIn;
//    module_data->continuousSamples = 0;
//    module_data->notes = notes;

    push_value_node(module_data, ui_node, graph);
    
    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }

    ui_nodes.push_back(ui_node);
    
}

void midiin_module::process(std::stack<void *> &value_stack)
{
    midiin_module_data *module_data = (midiin_module_data *)value_stack.top();
    value_stack.pop();
        
    value_stack.push(module_data);
}

void select_port(int port, RtMidiIn* rtmidi)
{
    rtmidi->openPort(port);
};

void midiin_module::show(const uinode2 &node, example::Graph<Node2> &graph)
{
    ImGui::PushItemWidth(150);
    ImNodes::BeginNode(node.id);
    
    midiin_module_data *module_data = (midiin_module_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
    
//    print("num edges", graph.num_edges_from_node(node.id), "from", node.id);
    
//    example::Span<const int> v = graph.neighbors(node.id);
//    for (auto it = v.begin(); it != v.end(); ++it) {
//        // if the current index is needed:
////        auto i = std::distance(v.begin(), it);
//
//        // access element as *it
//        print(*it);
//
//        // any code including continue, break, return
//    }
//
//    ImNodes::BeginNodeTitleBar();
//    char num_str[16];
//    char name[] = "rtmidi input";
//    sprintf(num_str, "%s (%d)", name, node.id);
//    ImGui::TextUnformatted(num_str);
//    ImNodes::EndNodeTitleBar();
    DEBUG_NODE_TITLE_BAR(node.type)
    
//    static int select_choice = 0;
    
    std::vector<std::string> item_names = module_data->port_names;
    
    const char *current_name = item_names[module_data->select_choice].c_str();

    if (ImGui::BeginCombo("##hidelabel", current_name)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < item_names.size(); n++)
        {
            bool is_selected = (current_name == item_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(item_names[n].c_str(), is_selected))
            {
                current_name = item_names[n].c_str();
//                print(current_name, "selected", n);
//                module_data->midiin_ptr->openPort(n);
                try
                {
                    module_data->midiin_ptr->closePort();

                    module_data->midiin_ptr->openPort(n);
                    module_data->midiin_ptr->setCallback(&midiCallback, module_data);
                    module_data->select_choice = n;
                }
                catch (RtMidiError &err)
                {
                    err.printMessage();
                    // return 1;
                }
            }
        }
        ImGui::EndCombo();
    }
    
    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
    ImGui::PopItemWidth();
}

void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData)
{
//    for(int i = 0; i < message->size(); i++)
//    {
//        std::cout << (int)message->at(i) << " ";
//    }
//    std::cout << std::endl;
    
    if (message->size() < 3)
    {
        return;
    }
    
    unsigned char command = message->at(0);
    unsigned char noteNum = message->at(1);
    unsigned char velocity = message->at(2);

    std::cout << (int)command << " " << (int)noteNum << " " << (int)velocity << std::endl;

//    UserData *userData = static_cast<UserData *>(pUserData);
    midiin_module_data *module_data = static_cast<midiin_module_data *>(pUserData);
    
//    module_data->notes.clear();
    
//    print("num consumers", module_data->num_consumers);

    if (command == 144) {
        MidiNoteMessage noteOnMsg;
        noteOnMsg.noteNum = noteNum;
        noteOnMsg.velocity = static_cast<float>(velocity) / 127.0f;
        noteOnMsg.isNoteOn = true;
        for(int i = 0; i < module_data->num_consumers; i++)
        {
            module_data->notesQueue.enqueue(noteOnMsg);
        }
    } else if (command == 128) {
        MidiNoteMessage noteOffMsg;
        noteOffMsg.noteNum = noteNum;
        noteOffMsg.velocity = static_cast<float>(velocity) / 127.0f;
        noteOffMsg.isNoteOn = false;
        for(int i = 0; i < module_data->num_consumers; i++)
        {
            module_data->notesQueue.enqueue(noteOffMsg);
        }

    }

}

double midi2Freq(int n)
{
    double f = 440.0 * pow(2, ((double)n - 69) / 12.0);

    return f;
}
