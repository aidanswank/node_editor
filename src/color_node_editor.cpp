#include "node_editor.h"
#include "graph.h"
#include "Oscillator.h"

#include <imgui/imnodes.h>
#include <imgui/imgui.h>

#include <SDL_keycode.h>
#include <SDL_timer.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>
#include "vprint.h"

namespace example
{
namespace
{
enum class NodeType
{
    add,
    multiply,
    divide,
    output,
    sine,
    time,
    value
};

enum class AudioNodeType
{
    value,
    sine,
    white,
    xfader,
    output 
};

// template<typename DataType>
struct Node
{
    NodeType type;
    float    value;

    explicit Node(const NodeType t) : type(t), value(0.f) {}

    Node(const NodeType t, const float v) : type(t), value(v) {}
};

int buffer_size = 256;

struct AudioNode
{
    AudioNodeType type;
    void *value;

    explicit AudioNode(const AudioNodeType t) : type(t) {
        value = new float[buffer_size]();
    }

    AudioNode(const AudioNodeType t, void* v) : type(t) {
        value = v;
    }
};

template<class T>
T clamp(T x, T a, T b)
{
    return std::min(b, std::max(x, a));
}

static float current_time_seconds = 0.f;
static bool  emulate_three_button_mouse = false;

// template<class T>
ImU32 evaluate(const Graph<Node>& graph, const int root_node)
{
    std::stack<int> postorder;

    dfs_traverse(
        graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });

    std::stack<float> value_stack;
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        const Node node = graph.node(id);

        switch (node.type)
        {
        case NodeType::add:
        {
            const float rhs = value_stack.top();
            value_stack.pop();
            const float lhs = value_stack.top();
            value_stack.pop();
            value_stack.push(lhs + rhs);
        }
        break;
        case NodeType::multiply:
        {
            const float rhs = value_stack.top();
            value_stack.pop();
            const float lhs = value_stack.top();
            value_stack.pop();
            value_stack.push(rhs * lhs);
        }
        break;
        case NodeType::divide:
        {
            const float rhs = value_stack.top();
            value_stack.pop();
            const float lhs = value_stack.top();
            value_stack.pop();
            value_stack.push(lhs / rhs);
            print("wtff",rhs,lhs);
        }
        break;
        case NodeType::sine:
        {
            const float x = value_stack.top();
            value_stack.pop();
            const float res = std::abs(std::sin(x));
            value_stack.push(res);
        }
        break;
        case NodeType::time:
        {
            value_stack.push(current_time_seconds);
        }
        break;
        case NodeType::value:
        {
            // If the edge does not have an edge connecting to another node, then just use the value
            // at this node. It means the node's input pin has not been connected to anything and
            // the value comes from the node's UI.
            if (graph.num_edges_from_node(id) == 0ull)
            {
                value_stack.push(node.value);
            }
            // print("node val",node.value);
        }
        break;
        case NodeType::output:
        {
            assert(value_stack.size() == 3ull);

            const int b = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
            value_stack.pop();
            const int g = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
            value_stack.pop();
            const int r = static_cast<int>(255.f * clamp(value_stack.top(), 0.f, 1.f) + 0.5f);
            value_stack.pop();
            return IM_COL32(r, g, b, 255);
        }
        break;
        default:
            break;
        }
    }

    // // The final output node isn't evaluated in the loop -- instead we just pop
    // // the three values which should be in the stack.
    // assert(value_stack.size() == 3ull);

    // i move it to the swtich statement to see if it would work, move back down later
}

float* audio_evaluate(const Graph<AudioNode>& graph, const int root_node)
{
    std::stack<int> postorder;
    float* res = new float[buffer_size]();

    dfs_traverse(graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });
    std::stack<void*> value_stack;
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        const AudioNode node = graph.node(id);

        switch (node.type)
        {
        case AudioNodeType::sine:
        {
            // print("sine");

            Oscillator *osc_ptr = (Oscillator*)value_stack.top();
            // print("yooo",gain_arr[0]);
            value_stack.pop();

            float* output = new float[buffer_size]();
            for(int i = 0; i < buffer_size; i++)
            {
                double osc_output =  osc_ptr->nextSample() * 0.06;

                // float whitenoise = rand() % 100;
                // whitenoise = whitenoise / 100;
                // output[i]  = whitenoise * 0.06;

                output[i] = osc_output;
            }
            value_stack.push(output);
        }
        break;
        case AudioNodeType::white:
        {
            float* output = new float[buffer_size]();
            for(int i = 0; i < buffer_size; i++)
            {

                float whitenoise = rand() % 100;
                whitenoise = whitenoise / 100;

                output[i]  = whitenoise * 0.06;

            }
            value_stack.push(output);
        }
        break;
        case AudioNodeType::xfader:
        {
            float* output = new float[buffer_size]();

            float* input_a = (float*)value_stack.top();
            value_stack.pop();

            float* input_b = (float*)value_stack.top();
            value_stack.pop();

            // float amount = 0.5;

            for(int i = 0; i < buffer_size; i++)
            {

                output[i]  = (input_a[i] * 0.5) + (input_b[i] * 0.5);
            }

            value_stack.push(output);
        }
        break;
        case AudioNodeType::value:
        {
            // print("value");
            // If the edge does not have an edge connecting to another node, then just use the value
            // at this node. It means the node's input pin has not been connected to anything and
            // the value comes from the node's UI.
            if (graph.num_edges_from_node(id) == 0ull)
            {
                value_stack.push((void*)node.value);
                // print("node val",node.value);
            }
        }
        break;
        case AudioNodeType::output:
            // print("out");
            // assert(value_stack.size() == 1ull);
            if(value_stack.size() == 2ull)
            {

                float *gain_arr = (float*)value_stack.top();
                // print("yooo",gain_arr[0]);
                value_stack.pop();

                res = (float*)value_stack.top();
                value_stack.pop();
     
                for(int i = 0; i < buffer_size; i++)
                {
                    res[i] *= (*gain_arr); // accessing [0]
                }

            }
        break;
        default:
            break;
        }
    }

    // The final output node isn't evaluated in the loop -- instead we just pop
    // the three values which should be in the stack.
    // assert(value_stack.size() == 1ull);

    // float* res = new float[buffer_size]();
    // if(value_stack.size() == 1ull)
    // {
    //     // print("yo");
    //     res = value_stack.top();
    //     // for(int i = 0; i < buffer_size; i++)
    //     // {
    //     //     print(res[i]);
    //     // }
    //     value_stack.pop();
    // }

    return res;
}

class ColorNodeEditor
{
public:
    ColorNodeEditor()
        : graph_(), nodes_(), root_node_id_(-1),
          minimap_location_(ImNodesMiniMapLocation_BottomRight)
    {
        audio_root_node_id_ = -1;
        printf("color node init\n");
    }

    void show()
    {
        // Update timer context
        current_time_seconds = 0.001f * SDL_GetTicks();

        auto flags = ImGuiWindowFlags_MenuBar;

        // The node editor window
        ImGui::Begin("color node editor", NULL, flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Mini-map"))
            {
                const char* names[] = {
                    "Top Left",
                    "Top Right",
                    "Bottom Left",
                    "Bottom Right",
                };
                int locations[] = {
                    ImNodesMiniMapLocation_TopLeft,
                    ImNodesMiniMapLocation_TopRight,
                    ImNodesMiniMapLocation_BottomLeft,
                    ImNodesMiniMapLocation_BottomRight,
                };

                for (int i = 0; i < 4; i++)
                {
                    bool selected = minimap_location_ == locations[i];
                    if (ImGui::MenuItem(names[i], NULL, &selected))
                        minimap_location_ = locations[i];
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Style"))
            {
                if (ImGui::MenuItem("Classic"))
                {
                    ImGui::StyleColorsClassic();
                    ImNodes::StyleColorsClassic();
                }
                if (ImGui::MenuItem("Dark"))
                {
                    ImGui::StyleColorsDark();
                    ImNodes::StyleColorsDark();
                }
                if (ImGui::MenuItem("Light"))
                {
                    ImGui::StyleColorsLight();
                    ImNodes::StyleColorsLight();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::TextUnformatted("Edit the color of the output color window using nodes.");
        ImGui::Columns(2);
        ImGui::TextUnformatted("A -- add node");
        ImGui::TextUnformatted("X -- delete selected node or link");
        ImGui::NextColumn();
        if (ImGui::Checkbox("emulate_three_button_mouse", &emulate_three_button_mouse))
        {
            ImNodes::GetIO().EmulateThreeButtonMouse.Modifier =
                emulate_three_button_mouse ? &ImGui::GetIO().KeyAlt : NULL;
        }
        ImGui::Columns(1);

        ImNodes::BeginNodeEditor();

        // Handle new nodes
        // These are driven by the user, so we place this code before rendering the nodes
        {
            const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                    ImNodes::IsEditorHovered() &&
                                    ImGui::IsKeyReleased(SDL_SCANCODE_A);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
            if (!ImGui::IsAnyItemHovered() && open_popup)
            {
                ImGui::OpenPopup("add node");
            }

            if (ImGui::BeginPopup("add node"))
            {
                const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

                // AUDIO SINE
                if (ImGui::MenuItem("AudioSine"))
                {
                    print("something happens");
                    // float* arr = new float[buffer_size]();
                    // const AudioNode value(AudioNodeType::value, arr);
                    // const AudioNode op(AudioNodeType::sine);

                    Oscillator *osc = new Oscillator();
                    osc->setMode(Oscillator::OSCILLATOR_MODE_SINE);
                    osc->setSampleRate(44100);

                    const AudioNode osc_node(AudioNodeType::value, (void*)osc);

                    AudioUiNode audio_ui_node;
                    audio_ui_node.type = AudioUiNodeType::sine;
                    audio_ui_node.id = audio_graph_.insert_node( AudioNode(AudioNodeType::sine) );
                    audio_ui_node.ui.sine.osc = audio_graph_.insert_node( osc_node ); // id of node

                    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);

                    audio_nodes_.push_back(audio_ui_node);

                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("AudioWhiteNoise"))
                {

                    AudioUiNode audio_ui_node;
                    audio_ui_node.type = AudioUiNodeType::white;
                    audio_ui_node.id = audio_graph_.insert_node( AudioNode(AudioNodeType::white) );
                    // audio_ui_node.ui.sine.osc = audio_graph_.insert_node( osc_node ); // id of node

                    // audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);

                    audio_nodes_.push_back(audio_ui_node);

                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("AudioXfader"))
                {
                    float* arr = new float[buffer_size]();
                    const AudioNode value(AudioNodeType::value, arr);
                    const AudioNode op(AudioNodeType::xfader);

                    AudioUiNode ui_node;
                    ui_node.type = AudioUiNodeType::xfader;
                    ui_node.ui.xfader.input_a = audio_graph_.insert_node(value);
                    ui_node.ui.xfader.input_b = audio_graph_.insert_node(value);
                    ui_node.id = audio_graph_.insert_node(op);

                    audio_graph_.insert_edge(ui_node.id, ui_node.ui.xfader.input_a);
                    audio_graph_.insert_edge(ui_node.id, ui_node.ui.xfader.input_b);

                    audio_nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("AudioOutput") && audio_root_node_id_ == -1)
                {
                    print("audio ouput node");
                    float* arr = new float[buffer_size]();
                    const AudioNode value(AudioNodeType::value, arr);
                    float* gain_arr = new float[1]();
                    gain_arr[0] = 0.1234;
                    const AudioNode gain(AudioNodeType::value, gain_arr);
                    const AudioNode out(AudioNodeType::output);

                    AudioUiNode audio_ui_node;
                    audio_ui_node.type = AudioUiNodeType::output;
                    audio_ui_node.ui.output.input = audio_graph_.insert_node(value);
                    audio_ui_node.ui.output.gain = audio_graph_.insert_node(gain); // data storage
                    audio_ui_node.id = audio_graph_.insert_node(out);

                    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.output.input);
                    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.output.gain);

                    audio_nodes_.push_back(audio_ui_node);
                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
                    audio_root_node_id_ = audio_ui_node.id;
                }

                if (ImGui::MenuItem("add"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::add);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::add;
                    ui_node.ui.add.lhs = graph_.insert_node(value);
                    ui_node.ui.add.rhs = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.add.lhs);
                    graph_.insert_edge(ui_node.id, ui_node.ui.add.rhs);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("multiply"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::multiply);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::multiply;
                    ui_node.ui.multiply.lhs = graph_.insert_node(value);
                    ui_node.ui.multiply.rhs = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.multiply.lhs);
                    graph_.insert_edge(ui_node.id, ui_node.ui.multiply.rhs);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("divide"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::divide);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::divide;
                    ui_node.ui.divide.lhs = graph_.insert_node(value);
                    ui_node.ui.divide.rhs = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    // printf("hmmm%i\n",ui_node.id);
                    // std::cout << ui_node.id << std::endl;

                    graph_.insert_edge(ui_node.id, ui_node.ui.divide.lhs);
                    graph_.insert_edge(ui_node.id, ui_node.ui.divide.rhs);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("output") && root_node_id_ == -1)
                {
                    const Node value(NodeType::value, 0.f);
                    const Node out(NodeType::output);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::output;
                    ui_node.ui.output.r = graph_.insert_node(value);
                    ui_node.ui.output.g = graph_.insert_node(value);
                    ui_node.ui.output.b = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(out);

                    graph_.insert_edge(ui_node.id, ui_node.ui.output.r);
                    graph_.insert_edge(ui_node.id, ui_node.ui.output.g);
                    graph_.insert_edge(ui_node.id, ui_node.ui.output.b);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                    root_node_id_ = ui_node.id;
                }

                if (ImGui::MenuItem("sine"))
                {
                    const Node value(NodeType::value, 0.f);
                    const Node op(NodeType::sine);

                    UiNode ui_node;
                    ui_node.type = UiNodeType::sine;
                    ui_node.ui.sine.input = graph_.insert_node(value);
                    ui_node.id = graph_.insert_node(op);

                    graph_.insert_edge(ui_node.id, ui_node.ui.sine.input);

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                if (ImGui::MenuItem("time"))
                {
                    UiNode ui_node;
                    ui_node.type = UiNodeType::time;
                    ui_node.id = graph_.insert_node(Node(NodeType::time));

                    nodes_.push_back(ui_node);
                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }

        // for (const UiNode& node : nodes_)
        // {
        //     switch (node.type)
        //     {
        //     case UiNodeType::add:
        //     {
        //         const float node_width = 100.f;
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("add");
        //         ImNodes::EndNodeTitleBar();
        //         {
        //             ImNodes::BeginInputAttribute(node.ui.add.lhs);
        //             const float label_width = ImGui::CalcTextSize("left").x;
        //             ImGui::TextUnformatted("left");
        //             if (graph_.num_edges_from_node(node.ui.add.lhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat("##hidelabel", &graph_.node(node.ui.add.lhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.add.rhs);
        //             const float label_width = ImGui::CalcTextSize("right").x;
        //             ImGui::TextUnformatted("right");
        //             if (graph_.num_edges_from_node(node.ui.add.rhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat("##hidelabel", &graph_.node(node.ui.add.rhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginOutputAttribute(node.id);
        //             const float label_width = ImGui::CalcTextSize("result").x;
        //             ImGui::Indent(node_width - label_width);
        //             ImGui::TextUnformatted("result");
        //             ImNodes::EndOutputAttribute();
        //         }

        //         ImNodes::EndNode();
        //     }
        //     break;
        //     case UiNodeType::multiply:
        //     {
        //         const float node_width = 100.0f;
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("multiply");
        //         ImNodes::EndNodeTitleBar();

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.multiply.lhs);
        //             const float label_width = ImGui::CalcTextSize("left").x;
        //             ImGui::TextUnformatted("left");
        //             if (graph_.num_edges_from_node(node.ui.multiply.lhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.multiply.lhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.multiply.rhs);
        //             const float label_width = ImGui::CalcTextSize("right").x;
        //             ImGui::TextUnformatted("right");
        //             if (graph_.num_edges_from_node(node.ui.multiply.rhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.multiply.rhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginOutputAttribute(node.id);
        //             const float label_width = ImGui::CalcTextSize("result").x;
        //             ImGui::Indent(node_width - label_width);
        //             ImGui::TextUnformatted("result");
        //             ImNodes::EndOutputAttribute();
        //         }

        //         ImNodes::EndNode();
        //     }
        //     break;
        //     case UiNodeType::divide:
        //     {
        //         ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(0, 150, 50, 255));

        //         const float node_width = 100.0f;
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("divide");
        //         ImNodes::EndNodeTitleBar();

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.divide.lhs);
        //             const float label_width = ImGui::CalcTextSize("left").x;
        //             ImGui::TextUnformatted("left");
        //             if (graph_.num_edges_from_node(node.ui.multiply.lhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.divide.lhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.divide.rhs);
        //             const float label_width = ImGui::CalcTextSize("right").x;
        //             ImGui::TextUnformatted("right");
        //             if (graph_.num_edges_from_node(node.ui.divide.rhs) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.divide.rhs).value, 0.01f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginOutputAttribute(node.id);
        //             const float label_width = ImGui::CalcTextSize("result").x;
        //             ImGui::Indent(node_width - label_width);
        //             ImGui::TextUnformatted("result");
        //             ImNodes::EndOutputAttribute();
        //         }

        //         ImNodes::EndNode();

        //         ImNodes::PopColorStyle();
        //     }
        //     break;
        //     case UiNodeType::output:
        //     {
        //         const float node_width = 100.0f;
        //         ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(11, 109, 191, 255));
        //         ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(45, 126, 194, 255));
        //         ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(81, 148, 204, 255));
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("output");
        //         ImNodes::EndNodeTitleBar();

        //         ImGui::Dummy(ImVec2(node_width, 0.f));
        //         {
        //             ImNodes::BeginInputAttribute(node.ui.output.r);
        //             const float label_width = ImGui::CalcTextSize("r").x;
        //             ImGui::TextUnformatted("r");
        //             if (graph_.num_edges_from_node(node.ui.output.r) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.output.r).value, 0.01f, 0.f, 1.0f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.output.g);
        //             const float label_width = ImGui::CalcTextSize("g").x;
        //             ImGui::TextUnformatted("g");
        //             if (graph_.num_edges_from_node(node.ui.output.g) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.output.g).value, 0.01f, 0.f, 1.f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.output.b);
        //             const float label_width = ImGui::CalcTextSize("b").x;
        //             ImGui::TextUnformatted("b");
        //             if (graph_.num_edges_from_node(node.ui.output.b) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel", &graph_.node(node.ui.output.b).value, 0.01f, 0.f, 1.0f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }
        //         ImNodes::EndNode();
        //         ImNodes::PopColorStyle();
        //         ImNodes::PopColorStyle();
        //         ImNodes::PopColorStyle();
        //     }
        //     break;
        //     case UiNodeType::sine:
        //     {
        //         const float node_width = 100.0f;
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("sine");
        //         ImNodes::EndNodeTitleBar();

        //         {
        //             ImNodes::BeginInputAttribute(node.ui.sine.input);
        //             const float label_width = ImGui::CalcTextSize("number").x;
        //             ImGui::TextUnformatted("number");
        //             if (graph_.num_edges_from_node(node.ui.sine.input) == 0ull)
        //             {
        //                 ImGui::SameLine();
        //                 ImGui::PushItemWidth(node_width - label_width);
        //                 ImGui::DragFloat(
        //                     "##hidelabel",
        //                     &graph_.node(node.ui.sine.input).value,
        //                     0.01f,
        //                     0.f,
        //                     1.0f);
        //                 ImGui::PopItemWidth();
        //             }
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImGui::Spacing();

        //         {
        //             ImNodes::BeginOutputAttribute(node.id);
        //             const float label_width = ImGui::CalcTextSize("output").x;
        //             ImGui::Indent(node_width - label_width);
        //             ImGui::TextUnformatted("output");
        //             ImNodes::EndInputAttribute();
        //         }

        //         ImNodes::EndNode();
        //     }
        //     break;
        //     case UiNodeType::time:
        //     {
        //         ImNodes::BeginNode(node.id);

        //         ImNodes::BeginNodeTitleBar();
        //         ImGui::TextUnformatted("time");
        //         ImNodes::EndNodeTitleBar();

        //         ImNodes::BeginOutputAttribute(node.id);
        //         ImGui::Text("output");
        //         ImNodes::EndOutputAttribute();

        //         ImNodes::EndNode();
        //     }
        //     break;
        //     }
        // }

        for (const AudioUiNode& node : audio_nodes_)
        {
            switch (node.type)
            {
            case AudioUiNodeType::sine:
            {
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("sine");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginOutputAttribute(node.id);
                ImGui::Text("output");
                ImNodes::EndOutputAttribute();

                ImNodes::EndNode();
            }
            break;
            case AudioUiNodeType::white:
            {
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("white noise");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginOutputAttribute(node.id);
                ImGui::Text("output");
                ImNodes::EndOutputAttribute();

                ImNodes::EndNode();
            }
            break;
            case AudioUiNodeType::xfader:
            {
                const float node_width = 100.0f;
                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("xfader");
                ImNodes::EndNodeTitleBar();

                {
                    ImNodes::BeginInputAttribute(node.ui.xfader.input_a);
                    const float label_width = ImGui::CalcTextSize("a").x;
                    ImGui::TextUnformatted("a");
                    ImNodes::EndInputAttribute();
                }

                {
                    ImNodes::BeginInputAttribute(node.ui.xfader.input_b);
                    const float label_width = ImGui::CalcTextSize("b").x;
                    ImGui::TextUnformatted("b");
                    ImNodes::EndInputAttribute();
                }

                ImNodes::BeginOutputAttribute(node.id);
                ImGui::Text("out");
                ImNodes::EndOutputAttribute();

                ImNodes::EndNode();

            }
            break;
            case AudioUiNodeType::output:
            {
                const float node_width = 100.0f;

                ImNodes::BeginNode(node.id);

                ImNodes::BeginNodeTitleBar();
                ImGui::TextUnformatted("output");
                ImNodes::EndNodeTitleBar();

                ImNodes::BeginInputAttribute(node.ui.output.input);
                ImGui::Text("input");
                ImNodes::EndInputAttribute();

                ImGui::PushItemWidth(node_width);
                // // if (audio_graph_.num_edges_from_node(node.ui.output.gain) == 0ull)
                // // {
                //     print("value!!",audio_graph_.node(node.ui.output.gain).value[0]);
                // // }
                float* fa_gain_stupid = (float*)audio_graph_.node(node.ui.output.gain).value;
                ImGui::DragFloat("gain", &fa_gain_stupid[0], 0.01f, 0.f, 1.0f);
                ImGui::PopItemWidth();

                ImNodes::EndNode();
            }
            break;
            }
        }

        // // ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(200, 100, 200, 255));
        // for (const auto& edge : graph_.edges())
        // {

        //     // If edge doesn't start at value, then it's an internal edge, i.e.
        //     // an edge which links a node's operation to its input. We don't
        //     // want to render node internals with visible links.
        //     print(  
        //             "edgeid",edge.id,
        //             "fromtype", (int)graph_.node(edge.from).type,
        //             "fromval", graph_.node(edge.from).value,
        //             "totype", (int)graph_.node(edge.to).type,
        //             "toval", graph_.node(edge.to).value
        //         );
        //     if (graph_.node(edge.from).type != NodeType::value)
        //         continue; // skip 1 iteration of the loop, not linking below
        //     print("linking", edge.id, edge.from, edge.to);
        //     // print(  
        //     //         "LINKING fromtype", (int)graph_.node(edge.from).type,
        //     //         "LINKING fromval", graph_.node(edge.from).value,
        //     //         "LINKING totype", (int)graph_.node(edge.to).type,
        //     //         "LINKING toval", graph_.node(edge.to).value
        //     //     );
        //     // if (graph_.node(edge.from).type == NodeType::divide)
        //     // {
        //     //     print("divide from");
        //     //     // ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(200, 100, 200, 255));
        //     // }
        //     ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 255, 255, 255));
        //     ImNodes::Link(edge.id, edge.from, edge.to);
        //     // print("from", (int)graph_.node(edge.from).type, "to", (int)graph_.node(edge.to).type);
        //     ImNodes::PopColorStyle();
        // }
        // // ImNodes::PopColorStyle();        

        for (const auto& edge : audio_graph_.edges())
        {
            
            // audio_graph_.node(edge.from).type
            // If edge doesn't start at value, then it's an internal edge, i.e.
            // an edge which links a node's operation to its input. We don't
            // want to render node internals with visible links.
            // print("AUDIO NODE TYPE as INT",  (int)AudioNodeType::value);
            // print(  
            //         "edgeid",edge.id,
            //         "fromtype", (int)audio_graph_.node(edge.from).type,
            //         "fromval", audio_graph_.node(edge.from).value,
            //         "totype", (int)audio_graph_.node(edge.to).type,
            //         "toval", audio_graph_.node(edge.to).value
            //     );
            if (audio_graph_.node(edge.from).type != AudioNodeType::value){
                // print("skipped?");
                // continue; // skip 1 iteration of the loop, not linking below
            } else {
                ImNodes::Link(edge.id, edge.from, edge.to);
            }

        }

        ImNodes::MiniMap(0.2f, minimap_location_);
        ImNodes::EndNodeEditor();

        // // Handle new links
        // // These are driven by Imnodes, so we place the code after EndNodeEditor().

        // {
        //     int start_attr, end_attr;
        //     if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
        //     {
        //         const NodeType start_type = graph_.node(start_attr).type;
        //         const NodeType end_type = graph_.node(end_attr).type;

        //         const bool valid_link = start_type != end_type;
        //         if (valid_link)
        //         {
        //             // Ensure the edge is always directed from the value to
        //             // whatever produces the value
        //             if (start_type != NodeType::value)
        //             {
        //                 std::swap(start_attr, end_attr);
        //             }
        //             graph_.insert_edge(start_attr, end_attr);
        //         }
        //     }
        // }

        // audio node linking
        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                // print("hey!!!",start_attr, end_attr);
                const AudioNodeType start_type = audio_graph_.node(start_attr).type;
                const AudioNodeType end_type = audio_graph_.node(end_attr).type;

                const bool valid_link = start_type != end_type;
                if (valid_link)
                {
                    print("is valid link");
                    // Ensure the edge is always directed from the value to
                    // whatever produces the value
                    if (start_type != AudioNodeType::value)
                    {
                        std::swap(start_attr, end_attr);
                    }
                    audio_graph_.insert_edge(start_attr, end_attr);
                }
            }
        }


        // // Handle deleted links

        // {
        //     int link_id;
        //     if (ImNodes::IsLinkDestroyed(&link_id))
        //     {
        //         graph_.erase_edge(link_id);
        //     }
        // }

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                audio_graph_.erase_edge(link_id);
            }
        }

        // {
        //     const int num_selected = ImNodes::NumSelectedLinks();
        //     if (num_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_X))
        //     {
        //         static std::vector<int> selected_links;
        //         selected_links.resize(static_cast<size_t>(num_selected));
        //         ImNodes::GetSelectedLinks(selected_links.data());
        //         for (const int edge_id : selected_links)
        //         {
        //             graph_.erase_edge(edge_id);
        //         }
        //     }
        // }

        // audio nodes deleting links
        {
            const int num_selected = ImNodes::NumSelectedLinks();
            if (num_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_X))
            {
                static std::vector<int> selected_links;
                selected_links.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedLinks(selected_links.data());
                for (const int edge_id : selected_links)
                {
                    audio_graph_.erase_edge(edge_id);
                }
            }
        }

        // {
        //     const int num_selected = ImNodes::NumSelectedNodes();
        //     if (num_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_X))
        //     {
        //         static std::vector<int> selected_nodes;
        //         selected_nodes.resize(static_cast<size_t>(num_selected));
        //         ImNodes::GetSelectedNodes(selected_nodes.data());
        //         for (const int node_id : selected_nodes)
        //         {
        //             graph_.erase_node(node_id);
        //             auto iter = std::find_if(
        //                 nodes_.begin(), nodes_.end(), [node_id](const UiNode& node) -> bool {
        //                     return node.id == node_id;
        //                 });
        //             // Erase any additional internal nodes
        //             switch (iter->type)
        //             {
        //             case UiNodeType::add:
        //                 graph_.erase_node(iter->ui.add.lhs);
        //                 graph_.erase_node(iter->ui.add.rhs);
        //                 break;
        //             case UiNodeType::multiply:
        //                 graph_.erase_node(iter->ui.multiply.lhs);
        //                 graph_.erase_node(iter->ui.multiply.rhs);
        //                 break;
        //             case UiNodeType::divide:
        //                 graph_.erase_node(iter->ui.divide.lhs);
        //                 graph_.erase_node(iter->ui.divide.rhs);
        //                 break;
        //             case UiNodeType::output:
        //                 graph_.erase_node(iter->ui.output.r);
        //                 graph_.erase_node(iter->ui.output.g);
        //                 graph_.erase_node(iter->ui.output.b);
        //                 root_node_id_ = -1;
        //                 break;
        //             case UiNodeType::sine:
        //                 graph_.erase_node(iter->ui.sine.input);
        //                 break;
        //             default:
        //                 break;
        //             }
        //             nodes_.erase(iter);
        //         }
        //     }
        // }

        {
            const int num_selected = ImNodes::NumSelectedNodes();
            if (num_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_X))
            {
                static std::vector<int> selected_nodes;
                selected_nodes.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedNodes(selected_nodes.data());
                for (const int node_id : selected_nodes)
                {
                    audio_graph_.erase_node(node_id);
                    auto iter = std::find_if(
                        audio_nodes_.begin(), audio_nodes_.end(), [node_id](const AudioUiNode& node) -> bool {
                            return node.id == node_id;
                        });
                    // Erase any additional internal nodes
                    switch (iter->type)
                    {
                    case AudioUiNodeType::output:
                        audio_graph_.erase_node(iter->ui.output.input);
                        audio_root_node_id_ = -1;
                        break;
                    default:
                        break;
                    }
                    audio_nodes_.erase(iter);
                }
            }
        }

        ImGui::End();

        // The color output window

        // const ImU32 color =
        //     root_node_id_ != -1 ? evaluate(graph_, root_node_id_) : IM_COL32(255, 20, 147, 255);
    
        ImU32 color;
        // print("root node", root_node_id_);
        if(root_node_id_ != -1)
        {
            // color = evaluate(graph_, root_node_id_);
        } else {
            color = IM_COL32(255, 20, 147, 255); // default out
        }

        ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
        ImGui::Begin("output color");
        ImGui::End();
        ImGui::PopStyleColor();
    }

// private:
    enum class UiNodeType
    {
        add,
        multiply,
        divide,
        output,
        sine,
        time,
    };

    enum class AudioUiNodeType
    {
        output,
        sine,
        white,
        xfader
    };

    struct AudioUiNode
    {
        AudioUiNodeType type;
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
                int osc;
            } sine; 

            struct
            {
                int input_a;
                int input_b;
            } xfader;

        } ui;
    };

    struct UiNode
    {
        UiNodeType type;
        // The identifying id of the ui node. For add, multiply, sine, and time
        // this is the "operation" node id. The additional input nodes are
        // stored in the structs.
        int id;

        union
        {
            struct
            {
                int lhs, rhs;
            } add;

            struct
            {
                int lhs, rhs;
            } multiply;

            struct
            {
                int lhs, rhs;
            } divide;

            struct
            {
                int r, g, b;
            } output;

            struct
            {
                int input;
            } sine;
        } ui;
    };

    Graph<Node>            graph_;
    Graph<AudioNode>       audio_graph_;
    std::vector<UiNode>    nodes_;
    std::vector<AudioUiNode>    audio_nodes_;
    int                    root_node_id_;
    int                    audio_root_node_id_;
    ImNodesMiniMapLocation minimap_location_;
};

} // namespace

static ColorNodeEditor color_editor;

void NodeEditorInitialize()
{
    print("node editor init");
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
}

float* NodeEditorAudioCallback() 
{ 
    // print("weeee");
    float* audio_output = new float[buffer_size]();
    // for(int i = 0; i < buffer_size; i++)
    // {
    //     // std::cout << mb->tracks[0].stream[0][i] << std::endl;
    //     float whitenoise = rand() % 100;
    //     whitenoise = whitenoise / 100;
    //     audio_output[i]     = whitenoise * 0.06;
    // }
    if(color_editor.audio_root_node_id_ != -1)
    {
        audio_output = audio_evaluate(color_editor.audio_graph_, color_editor.audio_root_node_id_);
    }
    return audio_output;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
} // namespace example