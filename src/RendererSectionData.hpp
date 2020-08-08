#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "Vector.hpp"
#include "Section.hpp"
#include "AssetManager.hpp"

class World;

struct BlockLightness {
	unsigned char face[FaceDirection::none] = { 0,0,0,0,0,0 };
};

struct SectionsData {
	std::shared_ptr<Section> section;
	std::shared_ptr<Section> west;
	std::shared_ptr<Section> east;
	std::shared_ptr<Section> top;
	std::shared_ptr<Section> bottom;
	std::shared_ptr<Section> north;
	std::shared_ptr<Section> south;

	BlockId GetBlockId(const Vector &pos) const;
	
	BlockLightness GetLight(const Vector &pos) const;

	BlockLightness GetSkyLight(const Vector &pos) const;
};

struct RendererSectionData {
    std::vector<glm::mat4> models;
    std::vector<glm::vec4> textures;
	std::vector<float> textureLayers;
	std::vector<float> textureFrames;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> lights;
    size_t hash = 0;
    Vector sectionPos;
	bool forced = false;
};

RendererSectionData ParseSection(const SectionsData &sections);
