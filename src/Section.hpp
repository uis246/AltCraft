#pragma once

#include <vector>
#include <map>
#include <shared_mutex>

#include "Block.hpp"
#include "Vector.hpp"

class Section {
	struct lightData {
		uint8_t block[2048];
		uint8_t sky[2048];//Only when hasSkyLight==true
	};

	mutable std::shared_mutex mutex;
	uint16_t *blocks = nullptr;//Should host endian
	struct lightData *light = nullptr;//2K or 4K
	uint8_t bitsPerBlock, pow;
	std::vector<uint16_t> palette;

	Vector worldPosition;
	mutable size_t hash = 0;
	bool hasSkyLight = false;

    void CalculateHash() const;
	void ExpandBPB() noexcept;
public:
	Section(Vector pos, unsigned char bitsPerBlock, std::vector<unsigned short> palette, std::vector<uint8_t> blockData, const std::vector<unsigned char> &lightData, const std::vector<unsigned char> &skyData);
	~Section();
    
	Section() = default;
	
    BlockId GetBlockId(Vector pos) const;
		
    unsigned char GetBlockLight(Vector pos) const;

    unsigned char GetBlockSkyLight(Vector pos) const;

    void SetBlockId(Vector pos, BlockId value);
	void SetBlock(unsigned int num, uint16_t block);

    void SetBlockLight(Vector pos, unsigned char value);

    void SetBlockSkyLight(Vector pos, unsigned char value);

	Vector GetPosition() const;

    size_t GetHash() const;
};
