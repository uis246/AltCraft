#pragma once

class GameState;
class Render;
class NetworkClient;
class LoopExecutionTimeController;

enum class State {
	InitialLoading,
	Menu,
	Playing,
	Paused,
};

void RunGame();

State GetState();

void SetState(State newState);

GameState* GetGameState();

Render* GetRender();

NetworkClient* GetNetworkClient();

LoopExecutionTimeController *GetTime();
