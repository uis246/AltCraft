#include "RendererSectionData.hpp"

#include <array>

#include <glm/gtc/matrix_transform.hpp>

#include "World.hpp"

inline const BlockId& GetBlockId(const Vector& pos, const std::array<BlockId, 4096> &blockIdData) {
	return blockIdData[pos.y * 256 + pos.z * 16 + pos.x];
}

inline const BlockId& GetBlockId(int x, int y, int z, const std::array<BlockId, 4096> &blockIdData) {
	return blockIdData[y * 256 + z * 16 + x];
}

static const glm::vec4 poss[] = {
	{0, 0, 0, 1},
	{0, 0, 1, 1},
	{1, 0, 0, 1},
	{1, 0, 1, 1}
};

void AddFacesByBlockModel(RendererSectionData &data, const BlockFaces &model, const Vector &isp, bool visibility[FaceDirection::none], BlockLightness light, BlockLightness skyLight) {
	for (const auto &face : model.faces) {
		uint8_t block = _max(light.face[0], light.face[1], light.face[2], light.face[3], light.face[4], light.face[5]);
		uint8_t sky = _max(skyLight.face[0], skyLight.face[1], skyLight.face[2], skyLight.face[3], skyLight.face[4], skyLight.face[5]);
		if (face.visibility != FaceDirection::none) {
			FaceDirection direction = face.visibility;
			Vector directionVec = model.faceDirectionVector[direction];
			FaceDirection faceDirection = FaceDirection::none;
			for (int i = 0; i < FaceDirection::none; i++) {
				if (FaceDirectionVector[i] == directionVec) {
					faceDirection = FaceDirection(i);
					break;
				}
			}
			if (faceDirection == FaceDirection::none)
				continue;

			if (visibility[faceDirection])
				continue;
		}

		glm::mat4 toApply = model.transform * face.transform;
		for (const glm::vec4 &pos : poss) {
			data.verts.push_back(glm::vec3(toApply * pos) + isp.glm());
		}
//		uint16_t ph = (((isp.z<<4)|isp.x)<<8) | (isp.y);
		uint32_t xw = (face.x << 16) | face.w;
		uint32_t yh = (face.y << 16) | face.h;

		uint32_t pTLfUu = (((isp.z<<4)|isp.x) << 24) | (face.tint<<20) | (block<<16) | (face.frames<<10) | face.Uu;
		uint32_t hLlVv = (isp.y<<24) | (sky<<16) | (face.layer<<10) | face.Vv;
		//5+5+4+4=18
		//32-18=30-16=20-6=14
		//14-8=10-4=6
		data.quadInfo.push_back(xw);
		data.quadInfo.push_back(yh);
		data.quadInfo.push_back(pTLfUu);
		data.quadInfo.push_back(hLlVv);
	}
}

BlockFaces *GetInternalBlockModel(const BlockId& id, std::vector<std::pair<BlockId, BlockFaces*>> &idModels) {
    for (const auto& it : idModels) {
        if (it.first == id)
            return it.second;
    }
    idModels.push_back(std::make_pair(id, &AssetManager::GetBlockModelByBlockId(id)));
    return idModels.back().second;
}

std::array<bool[FaceDirection::none], 4096> GetBlockVisibilityData(const SectionsData &sections, std::vector<std::pair<BlockId, BlockFaces*>> &idModels) {
	std::array<bool[FaceDirection::none], 4096> arr;
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				Vector vec(x, y, z);

				BlockId blockIdDown = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::down]);
				BlockId blockIdUp = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::up]);
				BlockId blockIdNorth = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::north]);
				BlockId blockIdSouth = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::south]);
				BlockId blockIdWest = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::west]);
				BlockId blockIdEast = sections.GetBlockId(vec + FaceDirectionVector[FaceDirection::east]);

				auto blockModelDown = GetInternalBlockModel(blockIdDown, idModels);
				auto blockModelUp = GetInternalBlockModel(blockIdUp, idModels);
				auto blockModelNorth = GetInternalBlockModel(blockIdNorth, idModels);
				auto blockModelSouth = GetInternalBlockModel(blockIdSouth, idModels);
				auto blockModelWest = GetInternalBlockModel(blockIdWest, idModels);
				auto blockModelEast = GetInternalBlockModel(blockIdEast, idModels);
				
				arr[y * 256 + z * 16 + x][FaceDirection::down] = blockIdDown.id != 0 && !blockModelDown->faces.empty() && blockModelDown->isBlock;
				arr[y * 256 + z * 16 + x][FaceDirection::up] = blockIdUp.id != 0 && !blockModelUp->faces.empty() && blockModelUp->isBlock;
				arr[y * 256 + z * 16 + x][FaceDirection::north] = blockIdNorth.id != 0 && !blockModelNorth->faces.empty() && blockModelNorth->isBlock;
				arr[y * 256 + z * 16 + x][FaceDirection::south] = blockIdSouth.id != 0 && !blockModelSouth->faces.empty() && blockModelSouth->isBlock;
				arr[y * 256 + z * 16 + x][FaceDirection::west] = blockIdWest.id != 0 && !blockModelWest->faces.empty() && blockModelWest->isBlock;
				arr[y * 256 + z * 16 + x][FaceDirection::east] = blockIdEast.id != 0 && !blockModelEast->faces.empty() && blockModelEast->isBlock;
			}
		}
	}
	return arr;
}

std::array<BlockId, 4096> SetBlockIdData(const SectionsData &sections) {
	std::array<BlockId, 4096> blockIdData;
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				blockIdData[y * 256 + z * 16 + x] = sections.section->GetBlockId(Vector(x, y, z));
			}
		}
	}
	return blockIdData;
}

RendererSectionData ParseSection(const SectionsData &sections) {
	OPTICK_EVENT();
	RendererSectionData data;

	std::vector<std::pair<BlockId, BlockFaces*>> idModels;
	std::array<BlockId, 4096> blockIdData = SetBlockIdData(sections);
	std::array<bool[FaceDirection::none], 4096> blockVisibility = GetBlockVisibilityData(sections, idModels);
	std::string textureName;

	data.hash = sections.section->GetHash();
	data.sectionPos = sections.section->GetPosition();

	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				BlockId block = GetBlockId(x, y, z, blockIdData);
				if (block.id == 0)
					continue;

				Vector vec(x, y, z);

				BlockLightness light = sections.GetLight(vec);
				BlockLightness skyLight = sections.GetSkyLight(vec);

				BlockFaces *model = GetInternalBlockModel(block, idModels);
				AddFacesByBlockModel(data, *model, vec, blockVisibility[y * 256 + z * 16 + x], light, skyLight);
			}
		}
	}
	data.verts.shrink_to_fit();
	data.quadInfo.shrink_to_fit();

	return data;
}

BlockId SectionsData::GetBlockId(const Vector &pos) const {
	if (pos.x < 0)
		return east->GetBlockId(Vector(15, pos.y, pos.z));

	if (pos.x > 15)
		return west->GetBlockId(Vector(0, pos.y, pos.z));

	if (pos.y < 0)
		return bottom->GetBlockId(Vector(pos.x, 15, pos.z));

	if (pos.y > 15)
		return top->GetBlockId(Vector(pos.x, 0, pos.z));

	if (pos.z < 0)
		return south->GetBlockId(Vector(pos.x, pos.y, 15));

	if (pos.z > 15)
		return north->GetBlockId(Vector(pos.x, pos.y, 0));

	return section->GetBlockId(pos);
}

BlockLightness SectionsData::GetLight(const Vector &pos) const {
	BlockLightness lightness;
	static const Vector directions[] = {
		Vector(1,0,0),
		Vector(-1,0,0),
		Vector(0,1,0),
		Vector(0,-1,0),
		Vector(0,0,1),
		Vector(0,0,-1),
	};

	unsigned char self = section->GetBlockLight(pos);

	for (const Vector &dir : directions) {
		Vector vec = pos + dir;
		unsigned char dirValue = 0;

		if (vec.x < 0 || vec.x > 15 || vec.y < 0 || vec.y > 15 || vec.z < 0 || vec.z > 15) {
			if (vec.x < 0)
				dirValue = east->GetBlockLight(Vector(15, vec.y, vec.z));
			if (vec.x > 15)
				dirValue = west->GetBlockLight(Vector(0, vec.y, vec.z));
			if (vec.y < 0)
				dirValue = bottom->GetBlockLight(Vector(vec.x, 15, vec.z));
			if (vec.y > 15)
				dirValue = top->GetBlockLight(Vector(vec.x, 0, vec.z));
			if (vec.z < 0)
				dirValue = south->GetBlockLight(Vector(vec.x, vec.y, 15));
			if (vec.z > 15)
				dirValue = north->GetBlockLight(Vector(vec.x, vec.y, 0));
		}
		else
			dirValue = section->GetBlockLight(vec);

		dirValue = _max(self, dirValue);

		if (dir == directions[0])
			lightness.face[FaceDirection::east] = dirValue;
		if (dir == directions[1])
			lightness.face[FaceDirection::west] = dirValue;
		if (dir == directions[2])
			lightness.face[FaceDirection::up] = dirValue;
		if (dir == directions[3])
			lightness.face[FaceDirection::down] = dirValue;
		if (dir == directions[4])
			lightness.face[FaceDirection::south] = dirValue;
		if (dir == directions[5])
			lightness.face[FaceDirection::north] = dirValue;
	}
	return lightness;
}

BlockLightness SectionsData::GetSkyLight(const Vector &pos) const {
	BlockLightness lightness;
	static const Vector directions[] = {
		Vector(1,0,0),
		Vector(-1,0,0),
		Vector(0,1,0),
		Vector(0,-1,0),
		Vector(0,0,1),
		Vector(0,0,-1),
	};

	unsigned char self = section->GetBlockSkyLight(pos);

	for (const Vector &dir : directions) {
		Vector vec = pos + dir;
		unsigned char dirValue = 0;

		if (vec.x < 0 || vec.x > 15 || vec.y < 0 || vec.y > 15 || vec.z < 0 || vec.z > 15) {
			if (vec.x < 0)
				dirValue = east->GetBlockSkyLight(Vector(15, vec.y, vec.z));
			if (vec.x > 15)
				dirValue = west->GetBlockSkyLight(Vector(0, vec.y, vec.z));
			if (vec.y < 0)
				dirValue = bottom->GetBlockSkyLight(Vector(vec.x, 15, vec.z));
			if (vec.y > 15)
				dirValue = top->GetBlockSkyLight(Vector(vec.x, 0, vec.z));
			if (vec.z < 0)
				dirValue = south->GetBlockSkyLight(Vector(vec.x, vec.y, 15));
			if (vec.z > 15)
				dirValue = north->GetBlockSkyLight(Vector(vec.x, vec.y, 0));
		}
		else
			dirValue = section->GetBlockSkyLight(vec);

		dirValue = _max(self, dirValue);

		if (dir == directions[0])
			lightness.face[FaceDirection::east] = dirValue;
		if (dir == directions[1])
			lightness.face[FaceDirection::west] = dirValue;
		if (dir == directions[2])
			lightness.face[FaceDirection::up] = dirValue;
		if (dir == directions[3])
			lightness.face[FaceDirection::down] = dirValue;
		if (dir == directions[4])
			lightness.face[FaceDirection::south] = dirValue;
		if (dir == directions[5])
			lightness.face[FaceDirection::north] = dirValue;
	}
	return lightness;
}
