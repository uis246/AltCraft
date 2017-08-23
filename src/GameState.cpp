#include "GameState.hpp"
#include "Event.hpp"
#include <iomanip>

GameState::GameState(std::shared_ptr<NetworkClient> networkClient) : nc(networkClient) {
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void GameState::Update(float deltaTime) {
	if (g_IsGameStarted) {
		std::chrono::steady_clock clock;
		static auto timeOfPreviousSendedPacket(clock.now());
		auto delta = clock.now() - timeOfPreviousSendedPacket;
		using namespace std::chrono_literals;
		if (delta >= 50ms) {
			auto packetToSend = std::make_shared<PacketPlayerPositionAndLookSB>(g_PlayerX, g_PlayerY, g_PlayerZ,
			                                                                    g_PlayerYaw,
			                                                                    g_PlayerPitch, g_OnGround);
			nc->SendPacket(packetToSend);
			timeOfPreviousSendedPacket = clock.now();
		}

		const float gravity = -9.8f;
		g_PlayerVelocityY += gravity * deltaTime;

		bool isCollides = world.isPlayerCollides(g_PlayerX, g_PlayerY + g_PlayerVelocityY * deltaTime,
		                                         g_PlayerZ);
		if (!isCollides) {
			g_PlayerY += g_PlayerVelocityY * deltaTime;
			g_OnGround = false;
		} else {
			g_PlayerVelocityY = 0;
			if (g_OnGround == false) {
				auto updatePacket = std::make_shared<PacketPlayerPosition>(g_PlayerX, g_PlayerY, g_PlayerZ, true);
				nc->SendPacket(updatePacket);
			}
			g_OnGround = true;
		}

		isCollides = world.isPlayerCollides(g_PlayerX + g_PlayerVelocityX * deltaTime, g_PlayerY,
		                                    g_PlayerZ + g_PlayerVelocityZ * deltaTime);
		if (!isCollides) {
			g_PlayerX += g_PlayerVelocityX * deltaTime;
			g_PlayerZ += g_PlayerVelocityZ * deltaTime;
        }

		const float AirResistance = 10.0f;
		glm::vec3 vel(g_PlayerVelocityX, 0, g_PlayerVelocityZ);
		glm::vec3 resistForce = -vel * AirResistance * deltaTime;
		vel += resistForce;
		g_PlayerVelocityX = vel.x;
		g_PlayerVelocityZ = vel.z;

        world.UpdatePhysics(deltaTime);
	}
}

void GameState::UpdatePacket()
{
    //Packet handling
    auto ptr = nc->ReceivePacket();
    while (ptr) {
        switch ((PacketNamePlayCB)ptr->GetPacketId()) {
        case SpawnObject: {
            auto packet = std::static_pointer_cast<PacketSpawnObject>(ptr);
            Entity entity = CreateObject(static_cast<ObjectType>(packet->Type));
            entity.entityId = packet->EntityId;
            entity.pos = VectorF(packet->X, packet->Y, packet->Z);
            entity.uuid = packet->ObjectUuid;
            entity.vel = Entity::DecodeVelocity(packet->VelocityX, packet->VelocityY, packet->VelocityZ);
            entity.yaw = packet->Yaw / 256.0;
            entity.pitch = packet->Pitch / 256.0;
            entity.renderColor = glm::vec3(0,1,0);
            world.AddEntity(entity);
            EventAgregator::PushEvent(EventType::EntityChanged, EntityChangedData{ entity.entityId });
            break;
        }
        case SpawnExperienceOrb:
            break;
        case SpawnGlobalEntity:
            break;
        case SpawnMob: {
            auto packet = std::static_pointer_cast<PacketSpawnObject>(ptr);
            Entity entity;
            entity.entityId = packet->EntityId;
            entity.pos = VectorF(packet->X, packet->Y, packet->Z);
            entity.uuid = packet->ObjectUuid;
            entity.vel = Entity::DecodeVelocity(packet->VelocityX, packet->VelocityY, packet->VelocityZ);
            entity.yaw = packet->Yaw / 256.0;
            entity.pitch = packet->Pitch / 256.0;
            entity.renderColor = glm::vec3(0,0,1);
            world.AddEntity(entity);
            EventAgregator::PushEvent(EventType::EntityChanged, EntityChangedData{ entity.entityId });
            break;
        }
        case SpawnPainting:
            break;
        case SpawnPlayer: {
            auto packet = std::static_pointer_cast<PacketSpawnPlayer>(ptr);
            Entity entity;
            entity.entityId = packet->EntityId;
            entity.pos = VectorF(packet->X, packet->Y, packet->Z);
            entity.uuid = packet->PlayerUuid;
            entity.yaw = packet->Yaw / 256.0;
            entity.pitch = packet->Pitch / 256.0;
            entity.renderColor = glm::vec3(1, 0, 0);
            entity.height = 1.8;
            entity.width = 0.6;
            world.AddEntity(entity);
            EventAgregator::PushEvent(EventType::EntityChanged, EntityChangedData{ entity.entityId });
            break;
        }
        case AnimationCB:
            break;
        case Statistics:
            break;
        case BlockBreakAnimation:
            break;
        case UpdateBlockEntity:
            break;
        case BlockAction:
            break;
        case BlockChange: {
            auto packet = std::static_pointer_cast<PacketBlockChange>(ptr);
            world.ParseChunkData(packet);
            break;
        }
        case BossBar:
            break;
        case ServerDifficulty:
            break;
        case TabCompleteCB:
            break;
        case ChatMessageCB:
            break;
        case MultiBlockChange: {
            auto packet = std::static_pointer_cast<PacketMultiBlockChange>(ptr);
            world.ParseChunkData(packet);
            break;
        }
        case ConfirmTransactionCB:
            break;
        case CloseWindowCB:
            break;
        case OpenWindow:
            break;
        case WindowItems:
            break;
        case WindowProperty:
            break;
        case SetSlot:
            break;
        case SetCooldown:
            break;
        case PluginMessageCB:
            break;
        case NamedSoundEffect:
            break;
        case DisconnectPlay: {
            auto packet = std::static_pointer_cast<PacketDisconnectPlay>(ptr);
            LOG(INFO) << "Disconnect reason: " << packet->Reason;
            EventAgregator::PushEvent(EventType::Disconnect, DisconnectData{ packet->Reason });
            break;
        }
        case EntityStatus:
            break;
        case Explosion:
            break;
        case UnloadChunk: {
            auto packet = std::static_pointer_cast<PacketUnloadChunk>(ptr);
            world.ParseChunkData(packet);
            break;
        }
        case ChangeGameState:
            break;
        case KeepAliveCB:
            LOG(WARNING) << "Receive KeepAlive packet in GameState handler";
            break;
        case ChunkData: {
            auto packet = std::static_pointer_cast<PacketChunkData>(ptr);
            world.ParseChunkData(packet);
            break;
        }
        case Effect:
            break;
        case Particle:
            break;
        case JoinGame: {
            auto packet = std::static_pointer_cast<PacketJoinGame>(ptr);
            g_PlayerEid = packet->EntityId;
            g_Gamemode = (packet->Gamemode & 0b11111011);
            g_Dimension = packet->Dimension;
            g_Difficulty = packet->Difficulty;
            g_MaxPlayers = packet->MaxPlayers;
            g_LevelType = packet->LevelType;
            g_ReducedDebugInfo = packet->ReducedDebugInfo;
            LOG(INFO) << "Gamemode is " << g_Gamemode << ", Difficulty is " << (int)g_Difficulty
                << ", Level Type is " << g_LevelType;
            EventAgregator::PushEvent(EventType::PlayerConnected, PlayerConnectedData{ gs });
            break;
        }
        case Map:
            break;
        case EntityRelativeMove: {
            auto packet = std::static_pointer_cast<PacketEntityRelativeMove>(ptr);
            Entity& entity = world.GetEntity(packet->EntityId);            
            entity.pos = entity.pos + Entity::DecodeDeltaPos(packet->DeltaX, packet->DeltaY, packet->DeltaZ);
            if (entity.entityId != 0)
                LOG(INFO) << "M: " << packet->EntityId;
            break;
        }
        case EntityLookAndRelativeMove: {
            auto packet = std::static_pointer_cast<PacketEntityLookAndRelativeMove>(ptr);
            Entity& entity = world.GetEntity(packet->EntityId);
            entity.pos = entity.pos + Entity::DecodeDeltaPos(packet->DeltaX, packet->DeltaY, packet->DeltaZ);
            entity.pitch = packet->Pitch / 256.0;
            entity.yaw = packet->Yaw / 256.0;            
            break;
        }
        case EntityLook: {
            auto packet = std::static_pointer_cast<PacketEntityLook>(ptr);
            Entity& entity = world.GetEntity(packet->EntityId);
            entity.pitch = packet->Pitch / 256.0;
            entity.yaw = packet->Yaw / 256.0;
            //LOG(INFO) << "L: " << packet->EntityId;
            break;
        }
        case EntityCB:
            break;
        case VehicleMove:
            break;
        case OpenSignEditor:
            break;
        case PlayerAbilitiesCB:
            break;
        case CombatEvent:
            break;
        case PlayerListItem:
            break;
        case PlayerPositionAndLookCB: {
            auto packet = std::static_pointer_cast<PacketPlayerPositionAndLookCB>(ptr);
            if ((packet->Flags & 0x10) != 0) {
                g_PlayerPitch += packet->Pitch;
            }
            else {
                g_PlayerPitch = packet->Pitch;
            };

            if ((packet->Flags & 0x08) != 0) {
                g_PlayerYaw += packet->Yaw;
            }
            else {
                g_PlayerYaw = packet->Yaw;
            }

            if ((packet->Flags & 0x01) != 0) {
                g_PlayerX += packet->X;
            }
            else {
                g_PlayerX = packet->X;
            }

            if ((packet->Flags & 0x02) != 0) {
                g_PlayerY += packet->Y;
            }
            else {
                g_PlayerY = packet->Y;
            }

            if ((packet->Flags & 0x04) != 0) {
                g_PlayerZ += packet->Z;
            }
            else {
                g_PlayerZ = packet->Z;
            }

            EventAgregator::PushEvent(EventType::PlayerPosChanged, PlayerPosChangedData{ VectorF(g_PlayerX,g_PlayerY,g_PlayerZ) });
            LOG(INFO) << "PlayerPos is " << g_PlayerX << ", " << g_PlayerY << ", " << g_PlayerZ << "\t\tAngle: "
                << g_PlayerYaw << "," << g_PlayerPitch;

            if (!g_IsGameStarted) {
                LOG(INFO) << "Game is started";
                EventAgregator::PushEvent(EventType::RemoveLoadingScreen, RemoveLoadingScreenData{});
            }

            g_IsGameStarted = true;

            auto packetResponse = std::make_shared<PacketTeleportConfirm>(packet->TeleportId);
            auto packetPerformRespawn = std::make_shared<PacketClientStatus>(0);

            nc->SendPacket(packetResponse);
            nc->SendPacket(packetPerformRespawn);
            break;
        }
        case UseBed:
            break;
        case UnlockRecipes:
            break;
        case DestroyEntities: {
            auto packet = std::static_pointer_cast<PacketDestroyEntities>(ptr);
            for (unsigned int entityId : packet->EntityIds) {
                world.DeleteEntity(entityId);
            }
            break;
        }
        case RemoveEntityEffect:
            break;
        case ResourcePackSend:
            break;
        case Respawn:
            break;
        case EntityHeadLook:
            break;
        case SelectAdvancementTab:
            break;
        case WorldBorder:
            break;
        case Camera:
            break;
        case HeldItemChangeCB:
            break;
        case DisplayScoreboard:
            break;
        case EntityMetadata:
            break;
        case AttachEntity:
            break;
        case EntityVelocity: {
            auto packet = std::static_pointer_cast<PacketEntityVelocity>(ptr);
            Entity& entity = world.GetEntity(packet->EntityId);
            entity.vel = Entity::DecodeVelocity(packet->VelocityX, packet->VelocityY, packet->VelocityZ);
            break;
        }
        case EntityEquipment:
            break;
        case SetExperience:
            break;
        case UpdateHealth: {
            auto packet = std::static_pointer_cast<PacketUpdateHealth>(ptr);
            g_PlayerHealth = packet->Health;
            if (g_PlayerHealth <= 0) {
                LOG(INFO) << "Player is dead. Respawning...";
                auto packetPerformRespawn = std::make_shared<PacketClientStatus>(0);
                nc->SendPacket(packetPerformRespawn);
            }
            break;
        }
        case ScoreboardObjective:
            break;
        case SetPassengers:
            break;
        case Teams:
            break;
        case UpdateScore:
            break;
        case SpawnPosition: {
            auto packet = std::static_pointer_cast<PacketSpawnPosition>(ptr);
            g_SpawnPosition = packet->Location;
            LOG(INFO) << "Spawn position is " << g_SpawnPosition.x << " " << g_SpawnPosition.y << " " << g_SpawnPosition.z;
            break;
        }
        case TimeUpdate: {
            auto packet = std::static_pointer_cast<PacketTimeUpdate>(ptr);
            WorldAge = packet->WorldAge;
            TimeOfDay = packet->TimeOfDay;
            break;
        }
        case Title:
            break;
        case SoundEffect:
            break;
        case PlayerListHeaderAndFooter:
            break;
        case CollectItem:
            break;
        case EntityTeleport: {
            auto packet = std::static_pointer_cast<PacketEntityTeleport>(ptr);
            Entity& entity = world.GetEntity(packet->EntityId);
            entity.pos = VectorF(packet->X, packet->Y, packet->Z);
            entity.pitch = packet->Pitch / 256.0;
            entity.yaw = packet->Yaw / 256.0;
            break;
        }
        case Advancements:
            break;
        case EntityProperties:
            break;
        case EntityEffect:
            break;
        }
        ptr = nc->ReceivePacket();
    }
}

void GameState::HandleMovement(GameState::Direction direction, float deltaTime) {
    if (!g_IsGameStarted)
        return;
	const float PlayerSpeed = 40.0;
	float velocity = PlayerSpeed * deltaTime;
	glm::vec3 vel(g_PlayerVelocityX, g_PlayerVelocityY, g_PlayerVelocityZ);
	glm::vec3 front(cos(glm::radians(this->Yaw())) * cos(glm::radians(this->Pitch())), 0,
	                sin(glm::radians(this->Yaw())) * cos(glm::radians(this->Pitch())));
	front = glm::normalize(front);
	glm::vec3 right = glm::normalize(glm::cross(front, this->WorldUp));
	switch (direction) {
		case FORWARD:
			vel += front * velocity;
			break;
		case BACKWARD:
			vel -= front * velocity;
			break;
		case RIGHT:
			vel += right * velocity;
			break;
		case LEFT:
			vel -= right * velocity;
			break;
		case JUMP:
			if (g_OnGround) {
				vel.y += 5;
				g_OnGround = false;
			}
			break;
	}
	g_PlayerVelocityX = vel.x;
	g_PlayerVelocityY = vel.y;
	g_PlayerVelocityZ = vel.z;
}

void GameState::HandleRotation(double yaw, double pitch) {
    if (!g_IsGameStarted)
        return;
	this->SetYaw(Yaw() + yaw);
	this->SetPitch(Pitch() + pitch);
	if (this->Pitch() > 89.0f)
		this->SetPitch(89.0f);
	if (this->Pitch() < -89.0f)
		this->SetPitch(-89.0f);
	this->updateCameraVectors();

	auto updatePacket = std::make_shared<PacketPlayerLook>(g_PlayerYaw, g_PlayerPitch, g_OnGround);
	nc->SendPacket(updatePacket);
}

glm::mat4 GameState::GetViewMatrix() {
    updateCameraVectors();
	auto pos = this->Position();
	pos.y += 1.62;
	return glm::lookAt(pos, pos + this->Front, this->Up);
}

void GameState::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw())) * cos(glm::radians(this->Pitch()));
	front.y = sin(glm::radians(this->Pitch()));
	front.z = sin(glm::radians(this->Yaw())) * cos(glm::radians(this->Pitch()));
	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}

float GameState::Yaw() {
	return g_PlayerYaw + 90;
}

float GameState::Pitch() {
	return -g_PlayerPitch;
}

void GameState::SetYaw(float yaw) {
	g_PlayerYaw = yaw - 90;
}

void GameState::SetPitch(float pitch) {
	g_PlayerPitch = -pitch;
}

glm::vec3 GameState::Position() {
	return glm::vec3(g_PlayerX, g_PlayerY, g_PlayerZ);
}

void GameState::SetPosition(glm::vec3 Position) {
	g_PlayerX = Position.x;
	g_PlayerY = Position.y;
	g_PlayerZ = Position.z;
}
