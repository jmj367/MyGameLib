#include "GBuffer.h"
#include <GL/glew.h>
#include "Texture.h"

GBuffer::GBuffer()
	:mBufferID(0)
{
	
}

GBuffer::~GBuffer()
{
	
}

bool GBuffer::Create(int width, int height)
{
	// フレームバッファを生成
	glGenFramebuffers(1, &mBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
	
	// 深度バッファを作成
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	
	// Gバッファ用のテクスチャを作成
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		// レンダリング用に作成
		Texture tex;
		tex.CreateForRendering(width, height, GL_RGB32F);
		mTextures.emplace_back(tex);
		// フレームバッファにアタッチ
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, tex.GetTextureID(), 0);
	}
	
	// アタッチメントのリストを作成
	std::vector<GLenum> attachments;
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}
	
	// 描画用バッファリストの取得
	glDrawBuffers(static_cast<GLsizei>(attachments.size()),
				  attachments.data());
	
	// フレームバッファの完成チェック
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Destroy();
		return false;
	}
	
	return true;
}

void GBuffer::Destroy()
{
	glDeleteFramebuffers(1, &mBufferID);
	for(int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		mTextures[i].Unload();
	}
	mTextures.clear();
}

void GBuffer::SetTexturesActive()
{
	for(int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		mTextures[i].SetActive(i);
	}
}