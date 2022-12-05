#include "App.h"

#define WinWidth 320 * 2
#define WinHeight 240 * 2

void App::Initialize()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // to make macos happy
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    SDL_Window *Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
    // assert(Window);
    if (Window == NULL)
    {
        printf("failed to create window!");
    }

    SDL_GLContext Context = SDL_GL_CreateContext(Window);

    SDL_GL_MakeCurrent(Window, Context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // std::cout << &Window << std::endl;

    mygui = new CustomImGui();
    mygui->Init(Window, Context);
    mygui->Attach(&running);
};

void App::Frame()
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        ImGui_ImplSDL2_ProcessEvent(&Event);
        if (Event.type == SDL_QUIT)
        {
            running = 0;
        }
    }

    mygui->NewFrame();
    mygui->Update();
    mygui->Render();
};

void App::Shutdown()
{
    std::cout << "main shutdown" << std::endl;
    mygui->Shutdown2();
    mygui->Shutdown();
};
