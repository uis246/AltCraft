#pragma once

#include <SDL.h>

#include "Shader.hpp"
#include "RendererWorld.hpp"

class Render {
    SDL_Window *window;
    SDL_GLContext glContext;

    bool renderGui = false;
    bool isRunning = true;
	bool isMouseCaptured = false;
    int prevMouseX, prevMouseY;
	float mouseXDelta, mouseYDelta;
    std::unique_ptr<RendererWorld> world; 
    bool renderWorld = false;
    RenderState renderState;
    LoopExecutionTimeController timer;
    std::map<SDL_Scancode, bool> isKeyPressed;
    bool HasFocus=true;
    float sensetivity = 0.1f;
    bool isWireframe = false;
    bool isDisplayInventory = false;

    enum GlobalState {
        InitialLoading,
        MainMenu,
        Loading,
        Playing,
        Paused,
    } state = InitialLoading;
    std::string stateString;

	void SetMouseCapture(bool IsCaptured);

	void HandleMouseCapture();

	void HandleEvents();

	void InitSfml(unsigned int WinWidth, unsigned int WinHeight, std::string WinTitle);

	void InitGlew();

	void RenderFrame();

	void PrepareToRendering();

    void UpdateKeyboard();

    void RenderGui();
public:
	Render(unsigned int windowWidth, unsigned int windowHeight, std::string windowTitle);
	~Render();

	void ExecuteRenderLoop();
};