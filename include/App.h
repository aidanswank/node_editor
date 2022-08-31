#include "myGui.h"

class App
{
public:
    bool running=true;

    void Initialize();
    void Frame();
    void Shutdown();
    void Run()
    {
        Initialize();
        while(running)
            Frame();
        Shutdown();
    };
    
    CustomImGui *mygui;
};