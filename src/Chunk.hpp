#pragma once

#include <memory>

#include "Section.hpp"
#include "Packet.hpp"

class Chunk{
	uint8_t biome[256];
	std::shared_ptr<Section> sections[16];

	const Vector2I32 pos;
	const bool hasSkyLight;

	Section* ParseSection(StreamInput *data, unsigned char height);
	void UpdateBlock(Vector blockPos, std::vector<Vector> *changedForce);
public:
	Chunk(Vector2I32 pos, bool hasSkyLight);
	void ParseChunk(PacketChunkData *packet);
	void ParseChunkData(PacketMultiBlockChange *packet);

	void Unload() noexcept;

	std::shared_ptr<Section> GetSectionShared(unsigned char height) const noexcept;
	Section* GetSection(unsigned char height) const noexcept;

	BlockId GetBlockId(Vector blockPos) const noexcept;
	void SetBlockId(Vector blockPos, BlockId block) noexcept;

	Vector2I32 GetPosition() const noexcept;
};
