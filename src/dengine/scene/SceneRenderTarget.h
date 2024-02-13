
#pragma once

#include <memory>
#include <map>
#include <string>

#include "dengine/data/RenderOptions.h"
#include "dengine/framebuffer/Framebuffer.h"

namespace Dg
{
/**
 * A set of framebuffers and their settings into which a Scene can draw.
 * This is a separate class to allow rendering a single scene with different settings
 * (requiring framebuffer(s) with different settings)
 */
class SceneRenderTarget
{
private:
	std::map<std::string, std::shared_ptr<Framebuffer>> m_framebuffers;
	std::weak_ptr<Framebuffer> outputFramebuffer;

	RenderOptions renderOptions;

public:
	std::weak_ptr<Framebuffer> getOutputFramebuffer();
	void setOutputFramebuffer(std::weak_ptr<Framebuffer> framebuffer);

	std::weak_ptr<Framebuffer> getFramebuffer(const std::string& name);
	void addFramebuffer(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer);

	void removeFramebuffer(const std::string& name);

	RenderOptions& getRenderOptions();
	void setRenderOptions(const RenderOptions& renderOptions);
};

} // namespace Dg
