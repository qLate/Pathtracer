#include "Renderer.h"

#include <iostream>

#include "Camera.h"
#include "GLObject.h"
#include "ImGUIHandler.h"
#include "Material.h"
#include "MyMath.h"

void Renderer::init()
{
	shaderP = new ShaderProgram<TraceShader>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	shaderP->use();

	shaderP->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	shaderP->setFloat2("pixelSize", ImGUIHandler::INIT_RENDER_SIZE);

	texArray = new GLTexture2DArray(TEX_ARRAY_BOUNDS.x, TEX_ARRAY_BOUNDS.y, TEX_ARRAY_BOUNDS.z, GL_RGBA8);
	shaderP->setFloat2("texArrayBounds", TEX_ARRAY_BOUNDS);
	shaderP->setInt("texArray", 0);

	Texture::defaultTex = new Texture("assets/textures/default.png");
	Material::defaultLit = new Material(Color::white(), true);
	Material::defaultUnlit = new Material(Color::white(), false);
}
void Renderer::uninit()
{
	delete shaderP;
	delete sceneViewFBO;
	delete texArray;
}

void Renderer::render()
{
	bindTextures();

	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFBO->id);

	shaderP->use();

	glBindVertexArray(shaderP->fragShader->vaoScreen->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::bindTextures()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray->id);
}

void Renderer::resizeView(glm::ivec2 size)
{
	delete sceneViewFBO;
	sceneViewFBO = new GLFrameBuffer(size.x, size.y);

	shaderP->setFloat2("pixelSize", size);
	Camera::instance->setSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);
}
