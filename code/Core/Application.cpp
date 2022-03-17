#include "Application.h"
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Techniques/Technique.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Skybox.h"

static void SaveScreenshot(Harm::Framebuffer* framebuffer)
{

    GLint w, h;

    
    framebuffer->Bind();
    framebuffer->BindColorTexture();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    

    std::vector<char> pixels;
    pixels.resize(w * h * 4);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    framebuffer->Unbind();
    stbi_flip_vertically_on_write(1);
    stbi_write_jpg("Screenshot.jpg", w, h, 4, pixels.data(), w * 4);
}


Application::Application(const std::string& title, int windowWidth, int windowHeight) :
    m_Window(nullptr),
    m_GLContext(nullptr),
    m_Running(false)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("%s\n", SDL_GetError());
        return;
    };
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_Window = SDL_CreateWindow(
        "Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );

    
    if (!m_Window)
        return;

    m_GLContext = SDL_GL_CreateContext(m_Window);

    if (!m_GLContext)
        return;

    if (!gladLoadGL())
        return;


    glEnable(GL_CULL_FACE);
    //SDL_GL_SetSwapInterval(0);

    m_Running = true;
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui_ImplSDL2_InitForOpenGL(m_Window, m_GLContext);
    ImGui_ImplOpenGL3_Init("#version 450");
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(m_GLContext);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Application::Run()
{    
    glm::vec2 mouse = glm::vec2(0.0f);
    enum { DIR_FORWARD, DIR_BACKWARD, DIR_LEFT, DIR_RIGHT };
    bool move[4] = {};
    float angleX = 0.0f;
    float angleY = 0.0f;
    float speed = 4.0f;
    float mouseSpeed = 50.0f;
    bool speedBump = false;
    bool freeFly = false;

    Uint64 oldTime = SDL_GetPerformanceCounter();
    Uint64 elaspedTime = 0;
    float dt = 0.0f;
    int fps = 0.0f;
    int FramesPerSecond = 0;
    int fpsCount = 0;
    float fpsTimer = 0.0f;
    float Timer = 0.0f;

    std::vector<Harm::Vertex> vertices = {
        { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
        { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
    };

    std::vector<u32> indices = { 0, 1, 2, 0, 2, 3 };

    Harm::Mesh fullscreenQuadMesh(vertices, indices, -1);
    Harm::Shader fullscreenShader("shaders/fullscreen.glsl");

    Harm::Framebuffer framebuffer(3840, 2160);
    Harm::Framebuffer shadowFramebuffer(2048, 2048);

    Harm::Camera mainCamera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    mainCamera.TranslateZ(-3);
    mainCamera.Update();

    Harm::LightTechnique lightTechnique;
    Harm::DepthTechnique depthTechnique;

    Harm::Skybox skybox("textures/cubemaps/kloppenheim_06/");

    Harm::Material defaultMaterial;

    Harm::DirectionalLight directionalLight;
    directionalLight.direction = glm::normalize(glm::vec3(-0.03f, -1.37f, -0.13f));

    std::vector<Harm::PointLight> pointLights;
    {
        Harm::PointLight pointLight;
        pointLight.position = glm::vec3(-12.0f, 3.0f, -4.5f);
        pointLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
        pointLights.push_back(pointLight);
    }

    #if 1
    Harm::Model model("models/Sponza.gltf");
    Harm::Transform modelTransform;
    modelTransform.SetScale(glm::vec3(0.01f));
    #else
    Harm::Model model("models/cube.fbx");
    Harm::Transform modelTransform;
    Harm::Model plane("models/room.fbx");
    Harm::Transform planeTransform;
    planeTransform.RotateX(-90.0f);
    planeTransform.TranslateZ(0.5f);
    planeTransform.Update();
    
    #endif
    float lightDistanceFactor = 20.0f;
    bool autoRotate = false;

    // Omnidirectional Shadow Mapping
    Harm::Shader depthCubeShader("shaders/depth_cube.glsl");
    glm::vec3 lightPos = pointLights[0].position;
    const s32 shadowSize = 1024;

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);


    GLuint depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (u32 i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float near = 1.0f;
    float far = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

    std::vector<glm::mat4> shadowTransforms = {
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, 1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,-1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, 0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    while (m_Running)
    {
        elaspedTime = SDL_GetPerformanceCounter() - oldTime;
        oldTime = SDL_GetPerformanceCounter();
        dt = static_cast<float>(elaspedTime) / SDL_GetPerformanceFrequency();
        fpsTimer += dt;
        fpsCount++;
        fps += static_cast<int>(1.0f / dt);
        Timer += dt;

        if (fpsTimer > 1.0f)
        {
            FramesPerSecond = fps / fpsCount;
            fps = 0;
            fpsCount = 0;
            fpsTimer = 0.0f;
            fps = 0.0f;
        }

        SDL_Event ev = {};
        mouse = glm::vec2(0.0f);
        while (SDL_PollEvent(&ev))
        {
            ImGui_ImplSDL2_ProcessEvent(&ev);
            if (ev.type == SDL_QUIT)
            {
                m_Running = false;
                break;
            }
            else if (ev.type == SDL_MOUSEMOTION)
            {
                mouse.x = static_cast<float>(ev.motion.xrel);
                mouse.y = static_cast<float>(ev.motion.yrel);
                glm::vec3 f = mainCamera.GetForwardVector();
            }
            else if (ev.type == SDL_KEYDOWN)
            {
                switch (ev.key.keysym.scancode)
                {
                case SDL_SCANCODE_W: move[DIR_FORWARD]  = true; break;
                case SDL_SCANCODE_A: move[DIR_LEFT]     = true; break;
                case SDL_SCANCODE_S: move[DIR_BACKWARD] = true; break;
                case SDL_SCANCODE_D: move[DIR_RIGHT]    = true; break;
                case SDL_SCANCODE_LSHIFT: speedBump = true; break;
                case SDL_SCANCODE_F8: SaveScreenshot(&framebuffer); break;
                }
            }
            else if (ev.type == SDL_KEYUP)
            {
                switch (ev.key.keysym.scancode)
                {
                case SDL_SCANCODE_W: move[DIR_FORWARD]  = false; break;
                case SDL_SCANCODE_A: move[DIR_LEFT]     = false; break;
                case SDL_SCANCODE_S: move[DIR_BACKWARD] = false; break;
                case SDL_SCANCODE_D: move[DIR_RIGHT]    = false; break;
                case SDL_SCANCODE_LSHIFT: speedBump = false; break;
                }
            }
            else if (ev.type == SDL_MOUSEBUTTONDOWN)
            {
                if (ev.button.button == SDL_BUTTON_RIGHT)
                {
                    freeFly = true;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }
            else if (ev.type == SDL_MOUSEBUTTONUP)
            {
                if (ev.button.button == SDL_BUTTON_RIGHT)
                {
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    freeFly = false;
                }
            }
        }

        // First person controls
        if (freeFly) {
            mainCamera.RotateX(-mouse.y * mouseSpeed * dt);
            mainCamera.RotateY(-mouse.x * mouseSpeed * dt);
            float s = speed * (speedBump ? 2.0f : 1.0f);
            if (move[DIR_FORWARD])  mainCamera.TranslateZ( s * dt);
            if (move[DIR_BACKWARD]) mainCamera.TranslateZ(-s * dt);
            if (move[DIR_RIGHT])    mainCamera.TranslateX( s * dt);
            if (move[DIR_LEFT])     mainCamera.TranslateX(-s * dt);
            
            mainCamera.Update();
        }

        if (autoRotate)
            modelTransform.RotateY(0.1f);
        modelTransform.Update();

        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
        // 1. Pass - Shadow Rendering Directional Light
        {
            shadowFramebuffer.Bind();
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            //glCullFace(GL_BACK);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::vec3 lightPosition = glm::normalize(directionalLight.direction) * -lightDistanceFactor;
            glm::mat4 lightProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f);
            glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

            depthTechnique.Enable();
            depthTechnique.SetProjectionAndViewMatrix(lightProjectionMatrix, lightViewMatrix);
            depthTechnique.GetShader()->SetUniformMat4(depthTechnique.GetShader()->GetUniformLocation("ModelMatrix"), modelTransform.GetWorldMatrix());

            model.Draw();

            glEnable(GL_CULL_FACE);

            shadowFramebuffer.Unbind();
            //glCullFace(GL_BACK);
        }

        // 2. Render Pass - Render point light shadows
        {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glViewport(0, 0, 1024, 1024);
            glDisable(GL_CULL_FACE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            depthCubeShader.Bind();
            
            depthCubeShader.SetUniformFloat(depthCubeShader.GetUniformLocation("FarPlane"), far);
            depthCubeShader.SetUniformVec3(depthCubeShader.GetUniformLocation("LightPosition"), lightPos);
            
            for (s32 i = 0; i < 6; i++)
            {
                depthCubeShader.SetUniformMat4(depthCubeShader.GetUniformLocation("ShadowMatrices["+ std::to_string(i) + "]"), shadowTransforms[i]);
            }

            depthCubeShader.SetUniformMat4(depthCubeShader.GetUniformLocation("ModelMatrix"), modelTransform.GetWorldMatrix());

            model.Draw();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_CULL_FACE);

        }


        // 3. Render Pass - Render the scene
        {
            framebuffer.Bind();
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



            skybox.SetProjectionViewMatrix(mainCamera.GetProjectionMatrix(), mainCamera.GetViewMatrix());
            skybox.Draw();

            lightTechnique.Enable();
            lightTechnique.SetProjectionAndViewMatrix(mainCamera.GetProjectionMatrix(), mainCamera.GetViewMatrix());
            lightTechnique.SetDirectionalLight(directionalLight);
            lightTechnique.SetPointLights(pointLights);

            lightTechnique.GetShader()->SetUniformMat4(lightTechnique.GetShader()->GetUniformLocation("LightSpaceMatrix"), lightSpaceMatrix);

            lightTechnique.GetShader()->SetUniformInt(lightTechnique.GetShader()->GetUniformLocation("DirectionLightShadowMap"), 3);
            shadowFramebuffer.BindDepthTexture(3);


            lightTechnique.GetShader()->SetUniformFloat(lightTechnique.GetShader()->GetUniformLocation("FarPlane"), far);
            lightTechnique.GetShader()->SetUniformInt(lightTechnique.GetShader()->GetUniformLocation("DepthMap"), 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            
            {
                const auto& meshes = model.GetMeshes();
                const auto& materials = model.GetMaterials();

                lightTechnique.GetShader()->SetUniformMat4(lightTechnique.GetShader()->GetUniformLocation("ModelMatrix"), modelTransform.GetWorldMatrix());

                for (const auto& mesh : meshes)
                {
                    if (mesh->GetMaterialIndex() != -1)
                    {
                        lightTechnique.SetMaterial(materials[mesh->GetMaterialIndex()]);
                    }
                    mesh->Draw();
                }
            }
           
            framebuffer.Unbind();
        }
        
        // 3. Render Pass
        glViewport(0, 0, 1920, 1080);
                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fullscreenShader.Bind();
        fullscreenShader.SetUniformInt(fullscreenShader.GetUniformLocation("DiffuseTexture"), 0);
        static bool showShadowMap = false;

        if (!showShadowMap)
            framebuffer.BindColorTexture();
        else
            shadowFramebuffer.BindColorTexture();
        fullscreenQuadMesh.Draw();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        glm::vec3 position = modelTransform.GetPosition();
        glm::vec3 rotation = modelTransform.GetRotation();
        glm::vec3 scale = modelTransform.GetScale();
        ImGui::Begin("Transform");
        ImGui::DragFloat3("Position", &position[0], 0.1f, -1000.0f, 1000.0f, "%.2f", 1.0f);
        ImGui::DragFloat3("Rotation", &rotation[0], 0.1f, -1000.0f, 1000.0f, "%.2f", 1.0f);
        ImGui::DragFloat3("Scale", &scale[0], 0.1f, -1000.0f, 1000.0f, "%.2f", 1.0f);
        ImGui::Checkbox("Auto Rotate", &autoRotate);
        ImGui::End();

        modelTransform.SetPositionAndRotation(position, rotation);
        modelTransform.SetScale(scale);

        ImGui::Begin("Directional Light");
        ImGui::DragFloat3("Direction", &directionalLight.direction[0], 0.001f, -glm::pi<float>(), glm::pi<float>(), "%.3f", 1.0f);
        ImGui::ColorEdit3("Color", &directionalLight.color[0], ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Ambient Intensity", &directionalLight.ambientIntensity, 0.05f, 0.0f, 1.0f, "%.2f", 1.0f);
        ImGui::DragFloat("Diffuse Intensity", &directionalLight.diffuseIntensity, 0.05f, 0.0f, 1.0f, "%.2f", 1.0f);
        ImGui::Checkbox("Show Shadowmap", &showShadowMap);
        ImGui::DragFloat("LightDistance", &lightDistanceFactor, 0.1f, 0.0f, 50.0f, "%.1f", 1.0f);
        ImGui::End();

        ImGui::Begin("Stats");
        glm::vec3 cp = mainCamera.GetPosition();
        ImGui::Text("Camera Position = %.2f, %.2f, %.2f", cp.x, cp.y, cp.z);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_Window);
    }
}

void Application::Quit()
{
    m_Running = false;
}
