#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <SDL.h>

#include "Utility.hpp"
#include "Renderer.hpp"
#include "Event.hpp"
#include "RendererUI.hpp"

class RendererWorld;
class Framebuffer;

class Render {
	RendererUI *ui;
	SDL_GLContext glContext;
	SDL_Window *window;

	bool renderGui = false;
	bool isMouseCaptured = false;
	int prevMouseX, prevMouseY;
	float mouseXDelta, mouseYDelta;
    std::unique_ptr<RendererWorld> world; 
    bool renderWorld = false;
    RenderState renderState;
    std::map<SDL_Scancode, bool> isKeyPressed;
    bool HasFocus=true;
    float sensetivity = 0.1f;
	bool isWireframe = false;
	std::unique_ptr<Framebuffer> framebuffer;
    std::vector<std::string> chatMessages;
	EventListener listener;
    std::string stateString;
	char fieldUsername[512];
	char fieldServerAddr[512];
	int fieldDistance;
	float fieldSensetivity;
	float fieldTargetFps;
	bool fieldWireframe;
	bool fieldVsync;
	bool fieldFlight;
	float fieldBrightness;
	float fieldResolutionScale;

	void SetMouseCapture(bool IsCaptured);

	void HandleMouseCapture();

	void HandleEvents();

	void InitSdl(unsigned int WinWidth, unsigned int WinHeight, std::string WinTitle);

	void InitGlew();

	void RenderFrame();

	void PrepareToRendering();

    void UpdateKeyboard();

	void RenderXhair(RenderState &state);
	void RenderGui(RenderState &state);

	void InitEvents();

public:
	Render(unsigned int windowWidth, unsigned int windowHeight, std::string windowTitle);
	~Render();

	void Update();
};
