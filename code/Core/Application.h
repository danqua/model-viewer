#pragma once

#include <glm/glm.hpp>
#ifdef _WIN32
#   include <SDL.h>
#   undef main
#endif

#include <string>

class Application
{
public:
    Application(const std::string& title, int windowWidth, int windowHeight);
    ~Application();
    void Run();
    void Quit();
private:
    SDL_Window* m_Window;
    SDL_GLContext m_GLContext;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;


    bool m_Running;
};