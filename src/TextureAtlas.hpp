#pragma once

#include <vector>

#include <GL/glew.h>

struct TextureData {
	std::vector<unsigned char> data; //expected format RGBA8888
	int width, height;
	size_t frames;
};

struct TextureCoord {
	double x, y, w, h;
	unsigned int pixelX, pixelY, pixelW, pixelH;
	size_t layer;
};

class TextureAtlas {
	GLuint textures[1];
	std::vector<TextureCoord> textureCoords;
public:
	TextureAtlas(std::vector<TextureData> &textures);

	TextureAtlas(const TextureAtlas &) = delete;

	~TextureAtlas();

	inline GLuint GetRawTextureId() {
		return textures[0];
	}
	inline GLuint GetRawTextureInfoId() {
		return textures[1];
	}

	TextureCoord GetTexture(int id) {
		return textureCoords[id];
	}
};
