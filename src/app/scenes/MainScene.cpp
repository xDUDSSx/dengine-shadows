#include "MainScene.h"

#include <memory>

#include "dengine/camera/OrbitCamera.h"
#include "dengine/lights/SunLight.h"
#include "dengine/entity/TexturedObject.h"
#include "Viewport/shader/Shaders.h"

#define PREVIEW_FOV 60
#define PREVIEW_CAMERA_X 50.0f

MainScene::MainScene() : Scene()
{
	m_orbitCamera = std::make_shared<Vp::OrbitCamera>();
	m_camera = m_orbitCamera;
}

void MainScene::init()
{
	Scene::init();

	m_orbitCamera->setFov(PREVIEW_FOV);
	m_orbitCamera->setRotationX(PREVIEW_CAMERA_X);

	// Lights
	Vp::SunLight* sun = new Vp::SunLight();
	sun->intensity = 0.8f;
	sun->color = glm::vec3(0.93, 0.98, 1.0);
	sun->direction = glm::vec3(-0.73, -0.64, -0.21);
	sun->pos = glm::vec3(0, 4, 0);
	m_lighting->addLight(sun);

	Vp::SunLight* sun2 = new Vp::SunLight();
	sun2->intensity = 0.08f;
	sun2->color = glm::vec3(0.804, 0.945, 1);
	sun2->specular = glm::vec3(0.0f); // Disable specular
	sun2->direction = glm::vec3(0.76, 0.58, 0.19);
	sun->pos = glm::vec3(0, 2, 0);
	m_lighting->addLight(sun2);

	Core::Mesh* mesh = RMI.mesh("Data/Models/camera.gltf");
	auto sceneModel = std::make_shared<Vp::TexturedObject>(mesh, Vp::Shaders::instance().m_phongShader.get());
	auto entity = addEntity(sceneModel);
	entity.lock()->m_highlight = true;
}