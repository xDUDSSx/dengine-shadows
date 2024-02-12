
#include "Shaders.h"

using namespace Dg;

#include "dengine/shader/BoxBlurShader.h"
#include "dengine/shader/ColorShader.h"
#include "dengine/shader/DepthShader.h"
#include "dengine/shader/ShadowShader.h"
#include "dengine/shader/GridShader.h"
#include "dengine/shader/PhongShader.h"
#include "dengine/shader/ScreenOverlayShader.h"
#include "dengine/shader/SelectionCompositeShader.h"
#include "dengine/shader/WBOITCompositeShader.h"
#include "dengine/shader/PSSMShader.h"

#include "dengine/platform/Logger.h"

bool Shaders::load()
{
	if (loaded)
		return true;

	bool ok = true;

	ok &= createShader<PhongShader>("Data/Shaders/phongVert.glsl", "Data/Shaders/phongFrag.glsl");
	ok &= createShader<ColorShader>("Data/Shaders/colorVert.glsl", "Data/Shaders/colorFrag.glsl");
	ok &= createShader<DepthShader>("Data/Shaders/depthVert.glsl", "");
	ok &= createShader<PSSMShader>("Data/Shaders/depthVert.glsl", "", "Data/Shaders/pssmGeo.glsl");
	ok &= createShader<ShadowShader>("Data/Shaders/shadowVert.glsl", "Data/Shaders/shadowFrag.glsl");
	ok &= createShader<GridShader>("Data/Shaders/gridVert.glsl", "Data/Shaders/gridFrag.glsl");
	ok &= createShader<WBOITCompositeShader>("Data/Shaders/basicVert.glsl", "Data/Shaders/wboitCompositeFrag.glsl");
	ok &= createShader<BoxBlurShader>("Data/Shaders/basicVert.glsl", "Data/Shaders/boxBlurFrag.glsl");
	ok &= createShader<SelectionCompositeShader>("Data/Shaders/basicVert.glsl", "Data/Shaders/selectionCompositeFrag.glsl");
	ok &= createShader<ScreenOverlayShader>("Data/Shaders/basicVert.glsl", "Data/Shaders/screenOverlayFrag.glsl");

	loaded = true;
	return ok;
}

bool Shaders::reload()
{
	if (!loaded)
	{
		load();
		return true;
	}

	RMI.m_forceReload = true;
	bool ok = true;
	for (const auto& [key, value] : m_shaders) {
		ok &= reloadShader(*value, value->m_vertSource, value->m_fragSource);
	}
	RMI.m_forceReload = false;

	return ok;
}

bool Shaders::reloadShader(Shader& shader, const std::string& vertSource, const std::string& fragSource)
{
	GLuint id = RMI.shader(vertSource, fragSource);
	if (id != 0)
	{
		shader.m_id = id;
		shader.init(true);
		return true;
	}
	else
	{
		return false;
	}
}

bool Shaders::checkForError(Shader& shader)
{
	return shader.m_id == 0;
}
