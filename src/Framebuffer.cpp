#include "Framebuffer.hpp"
#include "Shader.hpp"
#include <string>
#include "Utility.hpp"
#include "AssetManager.hpp"

const GLuint magic = 316784;
GLuint quadVao = magic, quadVbo = magic, Framebuffer::activeFBO = magic;
static GLuint currentWidth = 0, currentHeight = 0;

Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool createDepthStencilBuffer) : width(width), height(height) {
	OPTICK_EVENT();
	if (quadVao == magic) {
		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &quadVao);
		glGenBuffers(1, &quadVbo);
		glBindVertexArray(quadVao);
		glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		AssetTreeNode *fbo = AssetManager::GetAssetByAssetName("/altcraft/shaders/fbo");
		if (fbo->type == AssetTreeNode::ASSET_SHADER) {
			AssetShader *fboAsset = reinterpret_cast<AssetShader*>(fbo->asset.get());
			fboAsset->shader->Activate();
			fboAsset->shader->SetUniform("inputTexture", 1);
		}
		glActiveTexture(GL_TEXTURE1);
		glCheckError();
	}

	glGenTextures(1, &texColor);
	glBindTexture(GL_TEXTURE_2D, texColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCheckError();

	if (createDepthStencilBuffer) {
		glGenRenderbuffers(1, &rboDepthStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
	if(createDepthStencilBuffer)
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
	glCheckError();

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Failed to initialize framebuffer: " + std::to_string(framebufferStatus));	
}

Framebuffer::~Framebuffer() {
	if (rboDepthStencil)
		glDeleteRenderbuffers(1, &rboDepthStencil);
	if (texColor)
		glDeleteTextures(1, &texColor);

	glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::Activate() {
	OPTICK_EVENT();
	if (activeFBO != fbo || activeFBO == magic) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		activeFBO = fbo;
	}
	if (currentWidth != width || currentHeight != height)
		glViewport(0, 0, width, height);
}

void Framebuffer::RenderTo(Framebuffer &target) {
	OPTICK_EVENT();
	target.Activate();
	AssetTreeNode *fbo = AssetManager::GetAssetByAssetName("/altcraft/shaders/fbo");
	if (fbo->type == AssetTreeNode::ASSET_SHADER)
		reinterpret_cast<AssetShader*>(fbo->asset.get())->shader->Activate();
	glBindVertexArray(quadVao);
	glBindTexture(GL_TEXTURE_2D, texColor);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Framebuffer::Resize(unsigned int newWidth, unsigned int newHeight) {
	width = newWidth;
	height = newHeight;
	if (texColor) {
		glBindTexture(GL_TEXTURE_2D, texColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}	
	if (rboDepthStencil) {
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}
}

Framebuffer &Framebuffer::GetDefault() {
	OPTICK_EVENT();
	static char fboDefaultData[sizeof(Framebuffer)];
	static Framebuffer *fboDefault = nullptr;
	if (fboDefault == nullptr) {
		fboDefault = reinterpret_cast<Framebuffer*>(fboDefaultData);
		fboDefault->fbo = 0;
		fboDefault->width = 900;
		fboDefault->height = 480;
		fboDefault->texColor = 0;
		fboDefault->rboDepthStencil = 0;
	}
	return *fboDefault;
}

void Framebuffer::Clear(bool color, bool depth, bool stencil) {
	OPTICK_EVENT();
	Activate();
	GLbitfield clearBits = 0;
	if (color)
		clearBits |= GL_COLOR_BUFFER_BIT;
	if (depth)
		clearBits |= GL_DEPTH_BUFFER_BIT;
	if (stencil)
		clearBits |= GL_STENCIL_BUFFER_BIT;
	glClear(clearBits);
}
