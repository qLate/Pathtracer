#include "Pathtracer.h"

#include "Camera.h"
#include "GLObject.h"
#include "ImGUIHandler.h"

void Pathtracer::initTraceShader()
{
	shaderP = new ShaderProgram<TraceShader>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	shaderP->use();
	shaderP->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	shaderP->setFloat2("pixelSize", ImGUIHandler::INIT_RENDER_SIZE);
}

void Pathtracer::traceScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFBO->id);

	glBindTexture(GL_TEXTURE_2D, shaderP->fragShader->textures[0]->id);

	shaderP->use();
	glBindVertexArray(shaderP->fragShader->vaoScreen->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pathtracer::resizeView(glm::ivec2 size)
{
	delete sceneViewFBO;
	sceneViewFBO = new GLFrameBuffer(size.x, size.y);

	shaderP->setFloat2("pixelSize", size);
	Camera::instance->setSize({size.x / (float)size.y, 1});
}

void Pathtracer::uninit()
{
	delete shaderP;
	delete sceneViewFBO;
}

