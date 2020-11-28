#include "RendererWorld.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DebugInfo.hpp"
#include "Frustum.hpp"
#include "Event.hpp"
#include "AssetManager.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "GameState.hpp"
#include "Section.hpp"
#include "RendererSectionData.hpp"
#include "Game.hpp"
#include "Audio.hpp"

extern Section fallbackSection;

void RendererWorld::WorkerFunction(size_t workerId) {
	AC_THREAD_SET_NAME("Worker");
    EventListener tasksListener;

	tasksListener.RegisterHandler("ParseSection", [&](const Event &eventData) {
		OPTICK_EVENT("EV_ParseSection");
		auto data = eventData.get<std::tuple<size_t, size_t, bool>>();
		if (std::get<0>(data) != workerId)
			return;
		size_t id = std::get<1>(data);
		bool forced = std::get<2>(data);
		parsing[id].renderer = ParseSection(parsing[id].data);
		parsing[id].renderer.forced = forced;
		PUSH_EVENT("SectionParsed", id);
	});

    LoopExecutionTimeController timer(std::chrono::milliseconds(50));
    while (isRunning) {
        while (tasksListener.NotEmpty() && isRunning)
            tasksListener.HandleEvent();
        timer.Update();
    }
}

void RendererWorld::ParseQueueUpdate() {
	OPTICK_EVENT();
	while (!parseQueue.empty()) {
		size_t id = 0;
		for (; id < RendererWorld::parsingBufferSize && parsing[id].parsing; ++id) {}
		if (id >= RendererWorld::parsingBufferSize)
			break;

		Vector vec = parseQueue.front();
		parseQueue.pop();

		bool forced = false;

		if (vec.y > 4000) {
			forced = true;
			vec.y -= 4500;
		}
		
		GameState *gs = GetGameState();

		Vector2I32 chunkPos(vec.x, vec.z);

		parsing[id].chunkCenter = gs->GetWorld().GetChunkPtr(chunkPos),
		parsing[id].chunkNorth  = gs->GetWorld().GetChunkPtr(chunkPos+Vector2I32(0, 1)),
		parsing[id].chunkSouth  = gs->GetWorld().GetChunkPtr(chunkPos+Vector2I32(0, -1)),
		parsing[id].chunkWest   = gs->GetWorld().GetChunkPtr(chunkPos+Vector2I32(1, 0)),
		parsing[id].chunkEast   = gs->GetWorld().GetChunkPtr(chunkPos+Vector2I32(-1, 0));

		parsing[id].data.section = parsing[id].chunkCenter->GetSection(vec.y);
		parsing[id].data.north = parsing[id].chunkNorth->GetSection(vec.y);
		parsing[id].data.south = parsing[id].chunkSouth->GetSection(vec.y);
		parsing[id].data.west = parsing[id].chunkWest->GetSection(vec.y);
		parsing[id].data.east = parsing[id].chunkEast->GetSection(vec.y);
		parsing[id].data.bottom = parsing[id].chunkCenter->GetSection(vec.y-1);
		parsing[id].data.top = parsing[id].chunkCenter->GetSection(vec.y+1);

		if (parsing[id].data.section == nullptr)
			continue;//It's nothing to do

		if (parsing[id].data.north == nullptr)
			parsing[id].data.north = &fallbackSection;
		if (parsing[id].data.south == nullptr)
			parsing[id].data.south = &fallbackSection;
		if (parsing[id].data.west == nullptr)
			parsing[id].data.west = &fallbackSection;
		if (parsing[id].data.east == nullptr)
			parsing[id].data.east = &fallbackSection;
		if (parsing[id].data.bottom == nullptr)
			parsing[id].data.bottom = &fallbackSection;
		if (parsing[id].data.top == nullptr)
			parsing[id].data.top = &fallbackSection;


		parsing[id].parsing = true;

		PUSH_EVENT("ParseSection", std::make_tuple(currentWorker++, id, forced));
		if (currentWorker >= numOfWorkers)
			currentWorker = 0;
	}
}

void RendererWorld::ParseQeueueRemoveUnnecessary() {
	OPTICK_EVENT();
	size_t size = parseQueue.size();
	static std::vector<Vector> elements;
	elements.clear();
	elements.reserve(size);

	for (size_t i = 0; i < size; i++) {
		Vector vec = parseQueue.front();
		parseQueue.pop();

		if (vec.y > 4000) {
			parseQueue.push(vec);
			continue;
		}

		if (std::find(elements.begin(), elements.end(), vec) != elements.end())
			continue;//Continue if section already in queue
				
		const Section& section = GetGameState()->GetWorld().GetSection(vec);

		bool skip = false;

		for (size_t i = 0; i < RendererWorld::parsingBufferSize; i++) {
			if (parsing[i].data.section && parsing[i].data.section->GetPosition() == vec && (parsing[i].renderer.hash == section.GetHash() || parsing[i].parsing)) {
				skip = true;
				break;
			}
		}
		if (skip)
			continue;

		auto it = sections.find(vec);
		if (it != sections.end() && section.GetHash() == it->second.GetHash()) {
			continue;
		}

		parseQueue.push(vec);
		elements.push_back(vec);
	}

	parseQueueNeedRemoveUnnecessary = false;
}

void RendererWorld::UpdateAllSections(VectorF playerPos) {
	OPTICK_EVENT();
    Vector playerChunk(std::floor(GetGameState()->GetPlayer()->pos.x / 16), 0, std::floor(GetGameState()->GetPlayer()->pos.z / 16));

    std::vector<Vector> suitableChunks;
    auto chunks = GetGameState()->GetWorld().GetSectionsList();
    for (auto& it : chunks) {
        double distance = (Vector(it.x, 0, it.z) - playerChunk).GetLength();
        if (distance > MaxRenderingDistance)
            continue;
        suitableChunks.push_back(it);
    }

    std::vector<Vector> toRemove;

    for (auto& it : sections) {
        if (std::find(suitableChunks.begin(), suitableChunks.end(), it.first) == suitableChunks.end())
            toRemove.push_back(it.first);
    }

    for (auto& it : toRemove) {
		PUSH_EVENT("DeleteSectionRender", it);
    }

    playerChunk.y = std::floor(GetGameState()->GetPlayer()->pos.y / 16.0);
    std::sort(suitableChunks.begin(), suitableChunks.end(), [playerChunk](Vector lhs, Vector rhs) {
        double leftLengthToPlayer = (playerChunk - lhs).GetLength();
        double rightLengthToPlayer = (playerChunk - rhs).GetLength();
        return leftLengthToPlayer < rightLengthToPlayer;
    });

    for (auto& it : suitableChunks) {
		PUSH_EVENT("ChunkChanged", it);
    }	
}

RendererWorld::RendererWorld() {
	OPTICK_EVENT();
    MaxRenderingDistance = 2;
    numOfWorkers = _max(1, (signed int) std::thread::hardware_concurrency() - 2);

    listener = std::make_unique<EventListener>();

	globalTimeStart = std::chrono::high_resolution_clock::now();

    PrepareRender();
    
    listener->RegisterHandler("DeleteSectionRender", [this](const Event& eventData) {
		OPTICK_EVENT("EV_DeleteSectionRender");
		auto vec = eventData.get<Vector>();
        auto it = sections.find(vec);
        if (it == sections.end())
            return;
        sections.erase(it);
    });

    listener->RegisterHandler("SectionParsed",[this](const Event &eventData) {
		OPTICK_EVENT("EV_SectionParsed");
		auto id = eventData.get<size_t>();
		parsing[id].parsing = false;

		auto it = sections.find(parsing[id].renderer.sectionPos);

		if (it != sections.end()) {
			if (parsing[id].renderer.hash == it->second.GetHash() && !parsing[id].renderer.forced) {
				LOG(WARNING) << "Generated not necessary RendererSectionData: " << parsing[id].renderer.sectionPos;
				parsing[id] = RendererWorld::SectionParsing();
				return;
			}
			it->second.UpdateData(parsing[id].renderer);
		} else {
			if (!parsing[id].renderer.quadInfo.empty())
				sections.emplace(std::make_pair(parsing[id].renderer.sectionPos, RendererSection(parsing[id].renderer)));
		}

		parsing[id] = RendererWorld::SectionParsing();
    });
    
    listener->RegisterHandler("EntityChanged", [this](const Event& eventData) {
		OPTICK_EVENT("EV_EntityChanged");
		auto data = eventData.get<unsigned int>();
        for (unsigned int entityId : GetGameState()->GetWorld().GetEntitiesList()) {
            if (entityId == data) {
                entities.push_back(RendererEntity(entityId));
            }
        }
    });

    listener->RegisterHandler("ChunkChanged", [this](const Event& eventData) {
		OPTICK_EVENT("EV_ChunkChanged");
		auto vec = eventData.get<Vector>();
		if (vec == Vector())
			return;

        Vector playerChunk(std::floor(GetGameState()->GetPlayer()->pos.x / 16), 0, std::floor(GetGameState()->GetPlayer()->pos.z / 16));

        double distanceToChunk = (Vector(vec.x, 0, vec.z) - playerChunk).GetLength();
        if (MaxRenderingDistance != 1000 && distanceToChunk > MaxRenderingDistance) {
            return;
        }

		parseQueue.push(vec);

		parseQueueNeedRemoveUnnecessary = true;
    });

	listener->RegisterHandler("ChunkChangedForce", [this](const Event& eventData) {
		OPTICK_EVENT("EV_ChunkChangedForce");
		auto vec = eventData.get<Vector>();
		if (vec == Vector())
			return;

		Vector playerChunk(std::floor(GetGameState()->GetPlayer()->pos.x / 16), 0, std::floor(GetGameState()->GetPlayer()->pos.z / 16));

		double distanceToChunk = (Vector(vec.x, 0, vec.z) - playerChunk).GetLength();
		if (MaxRenderingDistance != 1000 && distanceToChunk > MaxRenderingDistance) {
			return;
		}

		vec.y += 4500;

		parseQueue.push(vec);

		parseQueueNeedRemoveUnnecessary = true;
	});

    listener->RegisterHandler("UpdateSectionsRender", [this](const Event&) {
        UpdateAllSections(GetGameState()->GetPlayer()->pos);
    });

    listener->RegisterHandler("PlayerPosChanged", [this](const Event& eventData) {
		auto pos = eventData.get<VectorF>();
        UpdateAllSections(pos);
    });

    listener->RegisterHandler("ChunkDeleted", [this](const Event& eventData) {
		auto pos = eventData.get<Vector>();
        auto it = sections.find(pos);
        if (it != sections.end())
            sections.erase(it);
    });

	listener->RegisterHandler("SetMinLightLevel", [this](const Event& eventData) {
		auto value = eventData.get<float>();
		AssetTreeNode *blockNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/face");
		if (blockNode->type==AssetTreeNode::ASSET_SHADER) {
			reinterpret_cast<AssetShader*>(blockNode->asset.get())->shader->Activate();
			reinterpret_cast<AssetShader*>(blockNode->asset.get())->shader->SetUniform("MinLightLevel", value);
		}

	});

	for (size_t i = 0; i < RendererWorld::parsingBufferSize; i++)
		parsing[i].data.section = nullptr;

	for (size_t i = 0; i < numOfWorkers; i++)
		workers.push_back(std::thread(&RendererWorld::WorkerFunction, this, i));

	PUSH_EVENT("UpdateSectionsRender", 0);
}

RendererWorld::~RendererWorld() {
    size_t faces = 0;
    for (auto& it : sections) {
        faces += it.second.numOfFaces;
    }
    LOG(INFO) << "Total faces to render: " << faces;
    isRunning = false;
    for (size_t i = 0; i < numOfWorkers; i++)
        workers[i].join();
    DebugInfo::renderSections = 0;
    DebugInfo::readyRenderer = 0;
}

void RendererWorld::Render(RenderState & renderState) {
	OPTICK_EVENT();
    //Common
    glm::mat4 projection = glm::perspective(
        glm::radians(70.0f), (float) renderState.WindowWidth / (float) renderState.WindowHeight,
        0.1f, 10000000.0f
    );
    glm::mat4 view = GetGameState()->GetViewMatrix();
	glm::mat4 projView = projection * view;

	{//Set listener position
	Entity *player = GetGameState()->GetPlayer();

	float playerYaw = Entity::DecodeYaw(player->yaw);
	float playerYawR = playerYaw * (M_PI / 180.f);

	float f = player->pitch * (M_PI / 180.f);
	float up = (player->pitch + 90.f) * (M_PI / 180.f);
	float cosYaw = cosf(playerYawR),
			sinYaw = sinf(playerYawR);
	float cosf0 = cosf(f),
			sinf0 = sinf(f);
	float cosUp = cosf(up),
			sinUp = sinf(up);

	Audio::UpdateListener(Vector3<float>(cosYaw*cosf0, sinf0, sinYaw*cosf0), Vector3<float>(cosUp*cosYaw, sinUp, cosUp*sinYaw), Vector3<float>(player->pos.x, player->pos.y+player->EyeOffset.y, player->pos.z), Vector3<float>(player->vel.x, player->vel.y, player->vel.z));
	}


    //Render Entities
    glLineWidth(3.0);
	Shader *entityShader = nullptr;
	AssetTreeNode *entityNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/entity");
	if (entityNode->type==AssetTreeNode::ASSET_SHADER)
		entityShader = reinterpret_cast<AssetShader*>(entityNode->asset.get())->shader.get();
	entityShader->Activate();
	entityShader->SetUniform("projView", projView);
	glCheckError();

    renderState.SetActiveVao(RendererEntity::GetVao());
    for (auto& it : entities) {
        it.Render(renderState, &GetGameState()->GetWorld());
    }
	glCheckError();

    //Render selected block
    const SelectionStatus& selectionStatus = GetGameState()->GetSelectionStatus();
    if (selectionStatus.isBlockSelected) {
        glLineWidth(2.0f);
        {
            glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, selectionStatus.selectedBlock.glm());
			model = glm::translate(model,glm::vec3(0.5f,0.5f,0.5f));
			model = glm::scale(model,glm::vec3(1.01f,1.01f,1.01f));
			entityShader->SetUniform("model", model);
			entityShader->SetUniform("color", glm::vec3(0, 0, 0));
			glDrawArrays(GL_LINES, 0, 24);
        }
	glCheckError();
    }

    //Render raycast hit
	const bool renderHit = false;
    if (renderHit) {
	VectorF hit = selectionStatus.raycastHit;
        glLineWidth(2.0f);
        {
            glm::mat4 model;
			model = glm::translate(model, hit.glm());
            model = glm::scale(model,glm::vec3(0.3f,0.3f,0.3f));
			entityShader->SetUniform("model", model);
			if (selectionStatus.isBlockSelected)
				entityShader->SetUniform("color", glm::vec3(0.7f, 0, 0));
            else
				entityShader->SetUniform("color", glm::vec3(0, 0, 0.7f));
            glDrawArrays(GL_LINE_STRIP, 0, 36);
        }
	glCheckError();
    }

	glLineWidth(1.0);

	//Render sky
	renderState.TimeOfDay = GetGameState()->GetTimeStatus().timeOfDay;
	Shader *skyShader = nullptr;
	AssetTreeNode *skyNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/sky");
	if (skyNode->type==AssetTreeNode::ASSET_SHADER)
		skyShader = reinterpret_cast<AssetShader*>(skyNode->asset.get())->shader.get();
	skyShader->Activate();
	skyShader->SetUniform("projView", projection);
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, GetGameState()->GetPlayer()->pos.glm());
	const float scale = 1000000.0f;
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	float shift = GetGameState()->GetTimeStatus().interpolatedTimeOfDay / 24000.0f;
	if (shift < 0)
		shift *= -1.0f;
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(360.0f * shift), glm::vec3(-1.0f, 0.0f, 0.0f));
	skyShader->SetUniform("model", model);

	glCheckError();

	const int sunriseMin = 22000;
	const int sunriseMax = 23500;
	const int moonriseMin = 12000;
	const int moonriseMax = 13500;
	const float sunriseLength = sunriseMax - sunriseMin;
	const float moonriseLength = moonriseMax - moonriseMin;

	float mixLevel = 0;
	float dayTime = GetGameState()->GetTimeStatus().interpolatedTimeOfDay;
	if (dayTime < 0)
		dayTime *= -1;
	while (dayTime > 24000)
		dayTime -= 24000;
	if ((dayTime > 0 && dayTime < moonriseMin) || dayTime > sunriseMax) //day
		mixLevel = 1.0;
	if (dayTime > moonriseMax && dayTime < sunriseMin) //night
		mixLevel = 0.0;
	if (dayTime >= sunriseMin && dayTime <= sunriseMax) //sunrise
		mixLevel = (dayTime - sunriseMin) / sunriseLength;
	if (dayTime >= moonriseMin && dayTime <= moonriseMax) { //moonrise
		float timePassed = (dayTime - moonriseMin);
		mixLevel = 1.0 - (timePassed / moonriseLength);
	}

	skyShader->SetUniform("DayTime", mixLevel);

	rendererSky.Render(renderState);
	glCheckError();

    //Render sections
	auto rawGlobalTime = (std::chrono::high_resolution_clock::now() - globalTimeStart);
	float globalTime = rawGlobalTime.count() / 1000000000.0f;
	Shader *blockShader = nullptr;
	AssetTreeNode *blockNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/face");
	if (blockNode->type==AssetTreeNode::ASSET_SHADER)
		blockShader = reinterpret_cast<AssetShader*>(blockNode->asset.get())->shader.get();
	blockShader->Activate();
	blockShader->SetUniform("DayTime", mixLevel);
	blockShader->SetUniform("projView", projView);
	blockShader->SetUniform("GlobalTime", globalTime);
	glCheckError();

	Frustum frustum(projView);

	size_t culledSections = sections.size();
	unsigned int renderedFaces = 0;
	for (auto& section : sections) {
		Vector pos = section.second.GetPosition() * 16;
		glm::vec3 point{
			pos.x + 8,
			pos.y + 8,
			pos.z + 8
		};

		bool isVisible = frustum.TestSphere(point, 16.0f);

		if (!isVisible) {
			culledSections--;
			continue;
		}

		section.second.Render();
		renderedFaces += section.second.numOfFaces;
	}
	glBindVertexArray(0);
    this->culledSections = culledSections;
	DebugInfo::renderFaces.store(renderedFaces, std::memory_order_relaxed);
    glCheckError();
}

void RendererWorld::PrepareRender() {
	Shader *blockShader = nullptr;
	AssetTreeNode *blockNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/face");
	if (blockNode->type==AssetTreeNode::ASSET_SHADER)
		blockShader = reinterpret_cast<AssetShader*>(blockNode->asset.get())->shader.get();
	blockShader->Activate();
	blockShader->SetUniform("textureAtlas", 0);
	blockShader->SetUniform("MinLightLevel", 0.2f);

	TextureCoord sunTexture = AssetManager::GetTexture("/minecraft/textures/environment/sun");
	TextureCoord moonTexture = AssetManager::GetTexture("/minecraft/textures/environment/moon_phases");
	moonTexture.w /= 4.0f; //First phase will be fine for now
	moonTexture.h /= 2.0f;

	Shader *sky = nullptr;
	AssetTreeNode *skyNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/sky");
	if (skyNode->type==AssetTreeNode::ASSET_SHADER)
		sky = reinterpret_cast<AssetShader*>(skyNode->asset.get())->shader.get();
	sky->Activate();
	sky->SetUniform("textureAtlas", 0);	
	sky->SetUniform("sunTexture", glm::vec4(sunTexture.x, sunTexture.y, sunTexture.w, sunTexture.h));
	sky->SetUniform("sunTextureLayer", (float)sunTexture.layer);
	sky->SetUniform("moonTexture", glm::vec4(moonTexture.x, moonTexture.y, moonTexture.w, moonTexture.h));
	sky->SetUniform("moonTextureLayer", (float)moonTexture.layer);
}

void RendererWorld::Update(double timeToUpdate) {
	OPTICK_EVENT();
    static auto timeSincePreviousUpdate = std::chrono::steady_clock::now();

	if (parseQueueNeedRemoveUnnecessary)
		ParseQeueueRemoveUnnecessary();

	ParseQueueUpdate();

	listener->HandleAllEvents();
    
    if (std::chrono::steady_clock::now() - timeSincePreviousUpdate > std::chrono::seconds(5)) {
		PUSH_EVENT("UpdateSectionsRender", 0);
        timeSincePreviousUpdate = std::chrono::steady_clock::now();
    }

	DebugInfo::readyRenderer.store(parseQueue.size(), std::memory_order_relaxed);
	DebugInfo::renderSections.store(sections.size(), std::memory_order_relaxed);
}
