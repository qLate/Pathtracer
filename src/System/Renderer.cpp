#include "Renderer.h"

#include <iostream>

#include "Camera.h"
#include "GLObject.h"
#include "ImGUIHandler.h"
#include "Material.h"
#include "MyMath.h"

void Renderer::init()
{
	renderProgram = new DefaultShaderProgram<TraceShader>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	renderProgram->use();
	renderProgram->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	renderProgram->setFloat2("pixelSize", ImGUIHandler::INIT_RENDER_SIZE);

	texArray = new GLTexture2DArray(TEX_ARRAY_BOUNDS.x, TEX_ARRAY_BOUNDS.y, TEX_ARRAY_BOUNDS.z, GL_RGBA8);
	renderProgram->setFloat2("texArrayBounds", glm::vec2(TEX_ARRAY_BOUNDS.x, TEX_ARRAY_BOUNDS.y));
	renderProgram->setInt("texArray", 0);

	Texture::defaultTex = new Texture("assets/textures/default.png");
	Material::defaultLit = new Material(Color::white(), true);
	Material::defaultUnlit = new Material(Color::white(), false);
	Material::debugLine = new Material(Color::blue(), false);
}
void Renderer::uninit()
{
	delete renderProgram;
	delete sceneViewFBO;
	delete texArray;
}

void Renderer::render()
{
	renderProgram->use();

	bindTextures();
	updateCameraUniforms();

	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFBO->id);

	glBindVertexArray(renderProgram->fragShader->vaoScreen->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::bindTextures()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray->id);
}
void Renderer::updateCameraUniforms()
{
	renderProgram->setFloat3("cameraPos", Camera::instance->getPos());
	renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->getRot()));
}

void Renderer::resizeView(glm::ivec2 size)
{
	delete sceneViewFBO;
	sceneViewFBO = new GLFrameBuffer(size.x, size.y);

	renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);
}
