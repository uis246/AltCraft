#include "RendererUI.hpp"

RendererUI::RendererUI() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(BUFCOUNT, VBOs);
}
RendererUI::~RendererUI() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(BUFCOUNT, VBOs);
}

void RendererUI::PrepareRender() noexcept {
	if(!dirtry && !permanentDirty)
		return;//All buffers up to date

	struct LayerStore *layer;
	layer = layers.data();

	//Generate geometry
	struct RenderBuffer rbuf;
	for(size_t i = min; i < max; i++) {
		layer[i].info.renderUpdate(&rbuf, layer[i].argument);
	}
	elementType = GL_UNSIGNED_SHORT;
	elements = rbuf.index.size();

	//Upload buffers
	glBindBuffer(GL_VERTEX_ARRAY, VBOs[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
	//TODO: buffer re-specification
	glBufferData(GL_VERTEX_ARRAY, rbuf.buffer.size() * sizeof(float), rbuf.buffer.data(), GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(GLushort), rbuf.index.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_VERTEX_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RendererUI::Render() noexcept {//Just draw
	glDrawElements(GL_TRIANGLES, elements, elementType, 0);
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
