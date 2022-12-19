#pragma once

enum class NodeType
{
    value,
    output,
    sine,
    white,
    xfader,
    waveviewer,
    vst,
    interface_in,
    test_external,
    midi_in,
};

//int buffer_size = 256;

struct Node2
{
    std::string type;
    void *value;

    explicit Node2(const std::string t) : type(t) {
//        value = new float[buffer_size]();
    }

    Node2(const std::string t, void* v) : type(t), value(v) {}
};

struct Node
{
    NodeType type;
    void *value;

    explicit Node(const NodeType t) : type(t) {
//        value = new float[buffer_size]();
    }

    Node(const NodeType t, void* v) : type(t), value(v) {}
};

// // ids of nodes that point to data
// struct test_ext
// {
//     int osc; // should rename osc_ptr or id_osc_ptr??
//     int osc_type;
//     int osc_output;
//     int freq;
// };

struct uinode2
{
    std::string type;
    int id;
    std::vector<int> ui;
};

#include "imgui.h"
struct node_module_base
{
    std::string type;
    using process_func = void(*)(std::stack<void *> &);
    using init_func = void(*)(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_);
    using show_func = void(*)(const uinode2 &, example::Graph<Node2> &);
    init_func init;
    show_func show;
    process_func process;
};

struct UiNode
{
    NodeType type;
    // The identifying id of the ui node. For add, multiply, sine, and time
    // this is the "operation" node id. The additional input nodes are
    // stored in the structs.
    int id;

    union
    {
        struct
        {
            int input;
            int gain;
        } output;
        
        struct
        {
            int osc; // should rename osc_ptr or id_osc_ptr??
            int osc_type;
            int osc_output;
            int freq;
        } sine;

        struct
        {
            int input_a;
            int input_b;
            int amount;
        } xfader;

        struct
        {
            int input;
            int view_buf;
        } waveviewer;

        struct
        {
            int input_buf;
            int vst_obj;
        } vst;
        
        struct
        {
            int input_buf;
        } interface_in;
        
        struct
        {
            int osc; // should rename osc_ptr or id_osc_ptr??
            int osc_type;
            int osc_output;
            int freq;
            int midiin_data;
        } test_external;
        
        struct
        {
            int midiin_ptr;
            int userdata_ptr;
        } midi_in;
        
//        test_ext myext;

    } ui;
};

//#include "test_module.h"
