#include <imgui/imnodes.h>
#include <imgui/imgui.h>

enum class NodeType
{
    output,
    sine,
    value
};

struct Node
{
    NodeType type;
    float *value;
};

struct Graph
{
    std::map<int, bool> visited;
    std::map<int, std::vector<int> > adj;
    std::map<int, Node> nodes;

    void addEdge(int v, int w)
    {
        adj[v].push_back(w);
    }

    void DFS(int v)
    {
        // mark the current node as visited and print it
        visited[v] = true;
        printf("%i ",v);

        if(adj[v].size()==0)
        {
            printf("end?\n");
        }
        
        for (int i = 0; i < adj[v].size(); i++)
        {
            int n = adj[v][i];
            if (!visited[n])
            {
                DFS(n);
            }
        }

    }
};

void node_space()
{
    ImGui::Begin("simple node editor");

    ImNodes::BeginNodeEditor();

    const int input_attr_id = 1;
    // ImNodes::SetNodeScreenSpacePos(3, ImVec2(100,130));
    ImNodes::BeginNode(input_attr_id);
    ImNodes::BeginInputAttribute(input_attr_id);
    static float dummy = 0;
    ImNodes::EndInputAttribute();
    ImGui::Text("output");
    ImGui::PushItemWidth(120);
    ImGui::SliderFloat("##volume", &dummy, 0.0, 1.0);
    const int output_attr_id = 2;
    ImNodes::BeginOutputAttribute(output_attr_id);
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    // ImNodes::SetNodeScreenSpacePos(3, ImVec2(225,208));

    ImNodes::BeginNode(2);
    ImGui::Text("im bill gates");
    ImNodes::BeginInputAttribute(3);
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(4);
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();
    // print("xy",ImGui::GetMousePos().x,ImGui::GetMousePos().y);

    ImNodes::Link(1, 2, 3);

    ImNodes::EndNodeEditor();

    ImGui::End();
}