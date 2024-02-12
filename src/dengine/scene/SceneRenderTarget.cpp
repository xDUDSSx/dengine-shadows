#include "SceneRenderTarget.h"

#include <utility>

#include "dengine/platform/Logger.h"

using namespace Dg;

std::weak_ptr<Framebuffer> SceneRenderTarget::getOutputFramebuffer()
{
	return outputFramebuffer;
}

void SceneRenderTarget::setOutputFramebuffer(std::weak_ptr<Framebuffer> framebuffer)
{
	outputFramebuffer = std::move(framebuffer);
}

RenderOptions& SceneRenderTarget::getRenderOptions()
{
	return renderOptions;
}

void SceneRenderTarget::setRenderOptions(const RenderOptions& renderOptions)
{
	this->renderOptions = renderOptions;
}

std::weak_ptr<Framebuffer> SceneRenderTarget::getFramebuffer(const std::string& name)
{
	auto it = m_framebuffers.find(name);
	if (it != m_framebuffers.end())
	{
		return it->second;
	}
	return {};
}

void SceneRenderTarget::addFramebuffer(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer)
{
	auto it = m_framebuffers.find(name);
	if (it != m_framebuffers.end())
	{
		LOG_ERROR("SceneRenderTarget: Cannot create framebuffer! A framebuffer with the name '{}' already exists.", name);
		return;
	}
	this->m_framebuffers.insert(std::make_pair(name, framebuffer));
}
