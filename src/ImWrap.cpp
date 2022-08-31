#include "ImWrap.h"
#include "vprint.h"

void ImWrap::Init(SDL_Window* p_Window, SDL_GLContext p_Context)
{ 
    Window = p_Window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext(); // node graph
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(Window, p_Context);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // glsl version

    font = io->Fonts->AddFontFromFileTTF("./res/fonts/unifont-14.0.01.ttf", 16.0f);

    // std::cout << &Window << std::endl;
}

void ImWrap::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(Window);
    // std::cout << &Window << std::endl;
    ImGui::NewFrame();
}

void ImWrap::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Swap buffers
    SDL_GL_SwapWindow(Window);

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

}

void ImWrap::Update()
{
    ImGui::PushFont(font);

    ImGui::ShowDemoWindow();

    ImGui::PopFont();
}

void ImWrap::Shutdown()
{
    print("shutdown1");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    ImNodes::DestroyContext();
}