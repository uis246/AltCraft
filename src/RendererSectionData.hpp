#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include "Vector.hpp"
#include "Section.hpp"
#include "AssetManager.hpp"

class World;

struct BlockLightness {
	unsigned char face[FaceDirection::none] = { 0,0,0,0,0,0 };
};

struct SectionsData {
	Section *section = nullptr;
	Section *west = nullptr;
	Section *east = nullptr;
	Section *top = nullptr;
	Section *bottom = nullptr;
	Section *north = nullptr;
	Section *south = nullptr;

	BlockId GetBlockId(const Vector &pos, bool locked = false) const;
	
	BlockLightness GetLight(const Vector &pos) const;

	BlockLightness GetSkyLight(const Vector &pos) const;
};

struct RendererSectionData {
	std::vector<glm::vec3> verts;
	std::vector<uint32_t> quadInfo;
	size_t hash = ~0;
    Vector sectionPos;
	bool forced = false;
};

RendererSectionData ParseSection(const SectionsData &sections);
