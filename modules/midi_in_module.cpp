//#include "midi_in_module.h"
//
//double midi2Freq(int n)
//{
//    double f = 440.0 * pow(2, ((double)n - 69) / 12.0);
//
//    return f;
//}
//
//void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData)
//{
//    if (message->size() < 3)
//    {
//        return;
//    }
//
//    unsigned char command = message->at(0);
//    unsigned char noteNum = message->at(1);
//    unsigned char velocity = message->at(2);
//
//    std::cout << (int)command << " " << (int)noteNum << " " << (int)velocity << std::endl;
//
//    UserData *userData = static_cast<UserData *>(pUserData);
//
//    if (command == 144) {
//            MidiNoteMessage noteOnMsg;
//            noteOnMsg.noteNum = noteNum;
//            noteOnMsg.velocity = static_cast<float>(velocity) / 127.0f;
//            noteOnMsg.isNoteOn = true;
//            userData->notesQueue.enqueue(noteOnMsg);
//    } else if (command == 128) {
//            MidiNoteMessage noteOffMsg;
//            noteOffMsg.noteNum = noteNum;
//            noteOffMsg.velocity = static_cast<float>(velocity) / 127.0f;
//            noteOffMsg.isNoteOn = false;
//            userData->notesQueue.enqueue(noteOffMsg);
//    }
//
//}
//
//void midiin_module_init(ImVec2 click_pos, example::Graph<Node> &audio_graph_, std::vector<UiNode> &ui_nodes_)
//{
//    UserData *userdata = new UserData;
//    userdata->continuousSamples = 0;
//
//    print("test module init");
//    RtMidiIn *midiIn = nullptr;
//    try
//    {
//        midiIn = new RtMidiIn();
//    }
//    catch (RtMidiError &err)
//    {
//        err.printMessage();
//        // return 1;
//    }
//    
//    try
//    {
//        midiIn->openPort(0);
//        midiIn->setCallback(&midiCallback, userdata);
//    }
//    catch (RtMidiError &err)
//    {
//        err.printMessage();
//        // return 1;
//    }
//    
//    const Node midiin_ptr_node(NodeType::value, (void *)midiIn);
//    const Node userdata_ptr_node(NodeType::value, (void *)userdata);
//
//    UiNode ui_node;
//    ui_node.type                          = NodeType::midi_in;
//    ui_node.id                            = audio_graph_.insert_node( Node( ui_node.type ) );
////    ui_node.ui.midi_in.midiin_ptr         = audio_graph_.insert_node( midiin_ptr_node );
//    ui_node.ui.midi_in.userdata_ptr       = audio_graph_.insert_node( userdata_ptr_node );
//
////    audio_graph_.insert_edge(ui_node.id, ui_node.ui.midi_in.midiin_ptr);
//    audio_graph_.insert_edge(ui_node.id, ui_node.ui.midi_in.userdata_ptr);
//    //
//    ui_nodes_.push_back(ui_node);
//
//    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
//    
////    ImNodes::SnapNodeToGrid(ui_node.id);
//
//
//};
//void midiin_module_process(std::stack<void *> &value_stack)
//{
//    UserData *userdata_ptr = (UserData *)value_stack.top();
//    value_stack.pop();
//    
////    RtMidiIn *midiin_ptr = (RtMidiIn *)value_stack.top();
////    value_stack.pop();
//    
//    value_stack.push(userdata_ptr);
//    
//};
//void midiin_module_show(const UiNode &node, example::Graph<Node> &audio_graph_)
//{
//    ImNodes::BeginNode(node.id);
//
//    ImNodes::BeginNodeTitleBar();
//    char num_str[16];
//    char name[] = "rtmidi input";
//    sprintf(num_str, "%s (%d)", name, node.id);
//    ImGui::TextUnformatted(num_str);
//    ImNodes::EndNodeTitleBar();
//    
//    ImNodes::BeginOutputAttribute(node.id);
//    ImGui::Text("output");
//    ImNodes::EndOutputAttribute();
//    
//    ImNodes::EndNode();
//
//};
