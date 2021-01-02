#include "RendererUI.hpp"
#include "UIHelper.hpp"

#include "AssetManager.hpp"
#include "Utility.hpp"
#include "Renderer.hpp"

#include <SDL.h>

RendererUI::RendererUI() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(BUFCOUNT, VBOs);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[BUFVERTS]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[BUFELEMENTS]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5*sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
RendererUI::~RendererUI() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(BUFCOUNT, VBOs);
}

void RendererUI::PrepareRender(RenderState &state) noexcept {
	struct LayerStore *layer;
	layer = layers.data();

	//Generate geometry
	struct RenderBuffer rbuf;
	rbuf.renderState = &state;
	for(size_t i = min; i < max; i++) {
		layer[i].info.renderUpdate(&rbuf, layer[i].argument);
	}
	elementType = GL_UNSIGNED_SHORT;
	elements = rbuf.index.size();

	//Upload buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[BUFVERTS]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[BUFELEMENTS]);
	//TODO: buffer re-specification
	//Assume that data will be updated every frame
	glBufferData(GL_ARRAY_BUFFER, rbuf.buffer.size() * sizeof(GLfloat), rbuf.buffer.data(), GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(GLushort), rbuf.index.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glCheckError();

	dirtry = false;
}

void RendererUI::Render(RenderState &state) noexcept {
	if(dirtry || permanentDirty)
		PrepareRender(state);//Update geometry if outdated

	glBindVertexArray(VAO);
	glDisable(GL_DEPTH_TEST);
	glCheckError();

	//Enable text shader
	Shader *textShader = nullptr;
	AssetTreeNode *textNode = AssetManager::GetAssetByAssetName("/altcraft/shaders/text");
	if (textNode->type==AssetTreeNode::ASSET_SHADER)
		textShader = reinterpret_cast<AssetShader*>(textNode->asset.get())->shader.get();
	textShader->Activate();

	glDrawElements(GL_TRIANGLES, elements, elementType, 0);
	glCheckError();

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glCheckError();
}




void RendererUI::UpdateRenderInfo() noexcept {
	dirtry = true;
	permanentDirty = false;

	struct LayerStore *layer = layers.data();
	size_t count = layers.size();

	if(!count) {
		//No UI to render
		//That's strange, but let's skip
		dirtry = false;
		min = 0;
		max = 0;
		return;
	}

	//Get layer render interval
	max = count;
	for(size_t i = count - 1;; i--) {
		min = i;
		currentLayer = layer[i].info.layer;
		permanentDirty |= layer[i].permadirt;
		if(currentLayer == MENU_ONLY)
			break;
		if(i == 0)
			break;
	}
}

void RendererUI::PopLayer() noexcept {
	layers.pop_back();

	UpdateRenderInfo();
}

void RendererUI::PushLayer(struct LayerInfo &layer, void *customArg, bool alwaysUpdate) {
	struct LayerStore newLayer;
	newLayer.info = layer;
	newLayer.argument = customArg;
	newLayer.permadirt = alwaysUpdate;
	layers.push_back(newLayer);

	UpdateRenderInfo();
}

void RendererUI::Redraw() noexcept {
	dirtry = true;
}

GLuint RendererUI::getVAO() noexcept {
	return VAO;
}




static TextureCoord white;

static uint8_t *glyphs;
static TextureCoord font[1];

UIHelper::UIHelper(struct RenderBuffer *buf) noexcept {
	buffer = buf;
	element = buf->buffer.size() / 9;
	vto = 16.f;
}

void UIHelper::InitHelper() noexcept {
	//Get white rect for colored render
	white = AssetManager::GetTexture("/altcraft/textures/gui/white");

	//Get sizes of charecters in texture
	AssetTreeNode *glyph_sizes = AssetManager::GetAssetByAssetName("/minecraft/font/glyph_sizes");
	glyphs = glyph_sizes->data.data();

	font[0] = AssetManager::GetTexture("/minecraft/textures/font/unicode_page_00");
}

void UIHelper::SetVerticalOffset(float offset) noexcept {
	vto = 16.f * offset;
}

struct Vertex {
	Vector2F pos;
	Vector3<float> uv;

	float r, g, b, a;

	void push(std::vector<GLfloat> *buf) {
		//Position
		buf->push_back(pos.x);
		buf->push_back(pos.z);

		//UV
		buf->push_back(uv.x);
		buf->push_back(uv.y);
		buf->push_back(uv.z);//Layer

		//Color
		buf->push_back(r);
		buf->push_back(g);
		buf->push_back(b);
		buf->push_back(a);
	}
};

void UIHelper::AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color) noexcept {
	Vector2<float> uv(white.x + white.w/2, white.y + white.h/2);

	Mat2x2F pos = {from, to};
	Mat2x2F UV = {uv, uv};

	AddRect(pos, UV, white.layer, color, .8f);
}

void UIHelper::AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color, const float opacity) noexcept {
	//Top-left
	struct Vertex vtx = {position[0],
				Vector3(uv[0].x, uv[0].z, (float)layer),
				color.x, color.y, color.z, opacity};
	vtx.push(&buffer->buffer);

	//Bottom-left
	vtx.pos = Vector2F(position[1].x, position[0].z);
	vtx.uv.x = uv[1].x;
	vtx.push(&buffer->buffer);

	//Top-right
	vtx.pos = Vector2F(position[0].x, position[1].z);
	vtx.uv = Vector3(uv[0].x, uv[1].z, (float)layer);
	vtx.push(&buffer->buffer);

	//Bottom-right
	vtx.pos = position[1];
	vtx.uv.x = uv[1].x;
	vtx.push(&buffer->buffer);


	buffer->index.push_back(element);
	buffer->index.push_back(element+1);
	buffer->index.push_back(element+2);
	buffer->index.push_back(element+2);
	buffer->index.push_back(element+1);
	buffer->index.push_back(element+3);

	element+=4;
}

const std::u16string UIHelper::ASCIIToU16(const std::string &str) noexcept {
	std::u16string result;
	size_t cnt = str.length();
	result.resize(cnt);
	const char *chars = str.c_str();
	for(size_t i = 0; i < cnt; i++) {
		result[i] = (uint16_t)chars[i];
	}

	return result;
}

const std::u16string UIHelper::UnicodeToU16(const std::string &str) noexcept {
	//FIXME: implement real unicode to u16 converter
	return ASCIIToU16(str);
}

Vector2F UIHelper::GetTextSize(const std::u16string &string, const float scale) noexcept {
	Vector2F offset, size;
	bool removeSpace = false;
	for(uint16_t chr : string) {
		if(chr == ' ') {
			offset.x += scale * 4;
		} else if (chr == '\n') {
			offset.x = 0;
			offset.z += scale * vto;
		} else {
			uint8_t endPixel, startPixel;
			{
				uint8_t glyphsz = glyphs[chr];
				endPixel = glyphsz & 0x0F;
				endPixel++;
				startPixel = glyphsz >> 4;
			}
			offset.x += scale * ((endPixel - startPixel) + 1);
			removeSpace = true;
		}
		if(offset.x > size.x)
			size.x = offset.x;
		if(offset.z > size.z)
			size.z = offset.z;
	}
	size.z += scale * 16;
	if(removeSpace)
		size.x -= scale;
	return size;
}
void UIHelper::AddText(const Vector2F position, const std::u16string &string, const float scale, const Vector3<float> color) noexcept {
	Vector2F offset;
	bool removeSpace = false;
	for(uint16_t chr : string) {
		if(chr == ' ') {
			if(removeSpace) {
				offset.x += scale * 6 / buffer->renderState->WindowWidth;
				removeSpace = false;
			} else
				offset.x += scale * 8 / buffer->renderState->WindowWidth;
		} else if (chr == '\n') {
			offset.x = 0;
			offset.z += scale * vto / buffer->renderState->WindowHeight;
			removeSpace = false;
		} else {
			uint8_t page = chr >> 8;
			uint8_t subchr = chr & 0xFF;

			uint8_t endPixel, startPixel;
			{
				uint8_t glyphsz = glyphs[chr];
				endPixel = glyphsz & 0x0F;
				endPixel++;
				startPixel = glyphsz >> 4;
			}
			uint8_t line = subchr / 16, colomn = subchr % 16;
			line = 15 - line;//Texture is flipped
			line *= 16;
			colomn *= 16;

			//Position of texture in atlas
			Vector2F texpos(font[page].x, font[page].y);
			Vector2F texsize(font[page].w, font[page].h);
			texsize = texsize * (1.f / 256);

			//Glyph size
			Vector2F charBox(endPixel - startPixel, 16.f);
			Vector2F charBase(colomn, line);

			//Coords in font texture
			Vector2F start = (charBase + Vector2F(startPixel, 0));
			Vector2F end = (charBase + Vector2F(endPixel, 16.f - 0.02f));

			//Convert to texture atlas coordinates
			start = (start * texsize) + texpos;
			end = (end * texsize) + texpos;

			//Prepare
			Mat2x2F uv = {start, end};
			Vector2F psz = charBox * 2 * scale / Vector2F(buffer->renderState->WindowWidth, buffer->renderState->WindowHeight);
			Mat2x2F positn = {position + offset, position + offset + psz};

			//Render
			AddRect(positn, uv, font[page].layer, color, 1.f);

			//Add horisontal glyph size to offset
			offset.x += psz.x;

			//Add space between glyphs
			offset.x += 2 * scale / buffer->renderState->WindowWidth;

			removeSpace = true;
		}
	}
}

void UIHelper::AddTextBox(const Vector2F from, const Vector2F pixelSize, const std::u16string &string, const float scale, const Vector3<float> backgroundColor, const Vector3<float> textColor) noexcept {
	AddColoredRect(from, from + (pixelSize * 2 * scale / (Vector2F(buffer->renderState->WindowWidth, buffer->renderState->WindowHeight))), backgroundColor);

	size_t idx;
	float width = 0;
	float maxWidth = pixelSize.x * scale - 4;
	bool removeSpace = false;

	for(size_t i = string.length(); i > 0; i--) {
		idx = i - 1;

		uint16_t chr = string[idx];
		if(chr == ' ')
			if(removeSpace) {
				width += 3 * scale;
				removeSpace = false;
			} else
				width += 4 * scale;
		else if(chr == '\n')
			break;
		else {
			uint8_t endPixel, startPixel;
			{
				uint8_t glyphsz = glyphs[chr];
				endPixel = glyphsz & 0x0F;
				endPixel++;
				startPixel = glyphsz >> 4;
			}
			width += scale * ((endPixel - startPixel) + 1);
			removeSpace = true;
		}

		if(width > maxWidth) {
			idx = i;
			break;
		}
	}
	AddText(from + (Vector2F(2.f * 2 / buffer->renderState->WindowWidth)), string.substr(idx, string.length() - idx), scale, textColor);
}

Vector2F UIHelper::GetCoord(const enum origin origin, Vector2F pixels) noexcept {
	Vector2F szHalf(buffer->renderState->WindowWidth, buffer->renderState->WindowHeight);
	Vector2F multiplier;
	switch (origin) {
		case CENTER:
			break;
		case UPLEFT:
			multiplier = Vector2F(-1, 1);
			break;
		case UPRIGHT:
			multiplier = Vector2F(1, 1);
			break;
		case DOWNRIGHT:
			multiplier = Vector2F(1, -1);
			break;
		case DOWNLEFT:
			multiplier = Vector2F(-1, -1);
			break;
		default:
			return Vector2F();
	}
	return multiplier + pixels * 2 / szHalf;
}


int UIHelper::TextInput = 0;
void UIHelper::StartTextEdit() noexcept {
	SDL_StartTextInput();
	TextInput = 1;
}
void UIHelper::StopTextEdit() noexcept {
	SDL_StopTextInput();
	TextInput = 0;
}
