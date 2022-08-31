struct Node
{
    int id;
    int num_inputs;
    int num_outputs;

    void display();
    float** process(float** input);
};

enum node_types
{ 
    utility,
    input_device,
    output_device,
};

// const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };

struct Link
{
    int id;
    int start_attr, end_attr;
};

// struct utilityNode : public Node
// {
//     virtual void display()
//     {

//     }
// };