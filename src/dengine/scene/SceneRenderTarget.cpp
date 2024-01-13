
#include "SceneRenderTarget.h"

#include <stdexcept>

using namespace Dg;

std::weak_ptr<Framebuffer> SceneRenderTarget::getOutputFramebuffer()
{
	return outputFramebuffer;
}

void SceneRenderTarget::setOutputFramebuffer(std::weak_ptr<Framebuffer> framebuffer)
{
	outputFramebuffer = framebuffer;
}

std::weak_ptr<Framebuffer> SceneRenderTarget::getFramebuffer(unsigned int index)
{
	try
	{
		return std::weak_ptr<Framebuffer>(m_framebuffers.at(index));
	}
	catch (const std::out_of_range&)
	{
		return std::weak_ptr<Framebuffer>();
	}
}

const RenderOptions& SceneRenderTarget::getRenderOptions() const
{
	return renderOptions;
}

void SceneRenderTarget::setRenderOptions(const RenderOptions& renderOptions)
{
	this->renderOptions = renderOptions;
}

void SceneRenderTarget::addFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
{
	this->m_framebuffers.push_back(framebuffer);
}
