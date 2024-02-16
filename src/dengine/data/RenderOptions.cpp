
#include "RenderOptions.h"
#include "dengine/shader/PhongShader.h"

using namespace Dg;

RenderOptions::RenderOptions()
{
	multisample = false;
	samples = 4;
	framebufferAlpha = false;
	wboit = false;
	wboitFunc = 0;
	selection = false;
	lightingModel = PhongShader::LightingModel::BLINN_PHONG;
	clearColor = glm::vec3(0.2f, 0.2f, 0.2f);

	shadows = false;
	shadowType = ShadowType::PSSM_INSTANCED;
	shadowCascadesCount = 4;
	pssmShadowsSplitSchemeWeight = 0.5f;
	shadowResolution = 1024;
	shadowBias = 0.0003f;
}
