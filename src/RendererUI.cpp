#include "RendererUI.hpp"
#include "UIHelper.hpp"

#include "AssetManager.hpp"
#include "Utility.hpp"
#include "Renderer.hpp"

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
	glBufferData(GL_ARRAY_BUFFER, rbuf.buffer.size() * sizeof(float), rbuf.buffer.data(), GL_STREAM_DRAW);
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
}

void UIHelper::InitHelper() noexcept {
	//Get white rect for colored render
	white = AssetManager::GetTexture("/altcraft/textures/gui/white");

	//Get sizes of charecters in texture
	AssetTreeNode *glyph_sizes = AssetManager::GetAssetByAssetName("/minecraft/font/glyph_sizes");
	glyphs = glyph_sizes->data.data();

	font[0] = AssetManager::GetTexture("/minecraft/textures/font/unicode_page_00");
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

void UIHelper::AddColoredRect(Vector2F from, Vector2F to, const Vector3<float> color) {
	Vector2<float> uv((white.x + white.w)/2, (white.y + white.h)/2);

	Mat2x2F pos = {from, to};
	Mat2x2F UV = {uv, uv};

	AddRect(pos, UV, white.layer, color);
}

void UIHelper::AddRect(Mat2x2F position, Mat2x2F uv, unsigned int layer, const Vector3<float> color) {
	//Top-left
	struct Vertex vtx = {position[0],
				Vector3(uv[0].x, uv[0].z, (float)layer),
				color.x, color.y, color.z, .8};
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

const std::u16string UIHelper::ASCIIToU16(std::string str) noexcept {
	std::u16string result;
	size_t cnt = str.length();
	result.resize(cnt);
	const char *chars = str.c_str();
	for(size_t i = 0; i < cnt; i++) {
		result[i] = (uint16_t)chars[i];
	}

	return result;
}

void UIHelper::GetTextSize(const std::u16string &string, Mat2x2F *returnSize) noexcept {

}
void UIHelper::AddText(Vector2F position, const std::u16string &string, const Vector3<float> color) {
	Vector2F offset;
	for(uint16_t chr : string) {
		uint8_t page = 0;//chr>>8
		uint8_t pos = chr&0xFF;

		uint8_t width = ((glyphs[chr/2] >> (4*chr%2)) & 0xF) + 1;
		uint8_t line = pos / 16, colomn = pos % 16;

		Vector2F texpos(font[page].x, font[page].y);
		Vector2F texsize(font[page].w, font[page].h);

		Vector2F start = Vector2F(colomn, line) * (16.f / 255);
		Vector2F end = start + (Vector2F(width, 15) * (1.f / 255));

		float a = 1 - start.z;
		float b = 1 - end.z;
		start.z = b;
		end.z = a;

		start = (start * texsize) + texpos;
		end = (end * texsize) + texpos;

		Mat2x2F uv = {start, end};
		Vector2F psz = Vector2F(width, 16) / Vector2F(buffer->renderState->WindowWidth, buffer->renderState->WindowHeight) * 8;
		Mat2x2F positn = {position + offset, position + offset + psz};

		AddRect(positn, uv, font[page].layer, color);

		offset.x += psz.x;
	}
}
