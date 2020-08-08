#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Block.hpp"
#include "Vector.hpp"

class Section {
	struct alignas(2048) light{
		uint8_t block[2048];
		uint8_t sky[];
	};
    std::vector<long long> block;
    std::vector<unsigned short> palette;
	std::shared_ptr<light> lightData;

	unsigned char bitsPerBlock = 0;
	bool hasSkyLight = false;

	Vector worldPosition;
    mutable size_t hash = 0;

    void CalculateHash() const;

    std::map<Vector, BlockId> overrideList;
public:
    Section(Vector pos, unsigned char bitsPerBlock, std::vector<unsigned short> palette, std::vector<long long> blockData, const std::vector<unsigned char> &lightData, const std::vector<unsigned char> &skyData);
    
	Section() = default;
	
    BlockId GetBlockId(Vector pos) const;
		
    unsigned char GetBlockLight(Vector pos) const;

    unsigned char GetBlockSkyLight(Vector pos) const;

    void SetBlockId(Vector pos, BlockId value);

    void SetBlockLight(Vector pos, unsigned char value);

    void SetBlockSkyLight(Vector pos, unsigned char value);

	Vector GetPosition() const;

    size_t GetHash() const;

//	void operator=(Section &sec) noexcept;
};
