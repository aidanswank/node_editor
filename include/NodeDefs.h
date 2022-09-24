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

    } ui;
};

int buffer_size = 256;

struct AudioNode
{
    NodeType type;
    void *value;

    explicit AudioNode(const NodeType t) : type(t) {
//        value = new float[buffer_size]();
    }

    AudioNode(const NodeType t, void* v) : type(t), value(v) {}
};
