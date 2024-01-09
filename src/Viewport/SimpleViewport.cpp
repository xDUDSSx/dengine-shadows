#include "SimpleViewport.h"

#include "dengine/resources/Shaper.h"

#include "Manipulators.h"
#include "Viewport/scene/scenes/PreviewScene.h"

void Vp::SimpleViewport::init()
{
	// Load shaders
	Shaders::instance().load();

	// Load shapes
	Shaper::initDefaultShapes();

	// Preload some useful models
	RMI.mesh("Data/Models/camera.gltf");

	// Create manipulators
//	m_manipulators = std::make_shared<Manipulators>(this);

	// Setup scenes
	m_scene = std::make_shared<PreviewScene>();

	m_scene->init();

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
