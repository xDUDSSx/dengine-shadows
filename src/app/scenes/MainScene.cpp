#include "MainScene.h"

#include <memory>

#include "dengine/camera/OrbitCamera.h"
#include "dengine/lights/SunLight.h"
#include "dengine/entity/TexturedObject.h"
#include "dengine/entity/ColoredObject.h"
#include "dengine/shader/Shaders.h"

MainScene::MainScene() : Scene()
{
	m_orbitCamera = std::make_shared<Dg::OrbitCamera>();
	m_camera = m_orbitCamera;
}

void MainScene::init()
{
	Scene::init();

	m_orbitCamera->setFov(90.f);
	m_orbitCamera->setRotationX(60);

	// Lights
	Dg::SunLight* sun = new Dg::SunLight();
	sun->intensity = 0.8f;
	sun->color = glm::vec3(0.93, 0.98, 1.0);
	sun->direction = glm::vec3(-0.73, -0.64, -0.21);
	sun->pos = glm::vec3(0, 4, 0);
	m_lighting->addLight(sun);

	Dg::SunLight* sun2 = new Dg::SunLight();
	sun2->intensity = 0.08f;
	sun2->color = glm::vec3(0.804, 0.945, 1);
	sun2->specular = glm::vec3(0.0f); // Disable specular
	sun2->direction = glm::vec3(0.76, 0.58, 0.19);
	sun->pos = glm::vec3(0, 2, 0);
	m_lighting->addLight(sun2);

	Core::Mesh* planeMesh = RMI.mesh("Data/Models/plane.gltf");
	Ptr<Dg::TexturedObject> plane = std::make_shared<Dg::TexturedObject>(planeMesh, Dg::Shaders::instance().m_phongShader.get());
	plane->m_modelMatrix = glm::rotate(plane->m_modelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
	plane->m_modelMatrix = glm::scale(plane->m_modelMatrix, glm::vec3(15.f));
	addEntity(plane);

	Core::Mesh* cameraMesh = RMI.mesh("Data/Models/camera.gltf");
	Ptr<Dg::TexturedObject> camera =
	    std::make_shared<Dg::TexturedObject>(cameraMesh, Dg::Shaders::instance().m_phongShader.get());
	camera->m_modelMatrix = glm::translate(camera->m_modelMatrix, glm::vec3(3.f, 1.0f, -2.0f));
	addEntity(camera);

	Core::Mesh* boxMesh = RMI.mesh("Data/Models/box_metal.gltf");
	Ptr<Dg::TexturedObject> metalBox = std::make_shared<Dg::TexturedObject>(boxMesh);
	metalBox->m_modelMatrix = glm::translate(metalBox->m_modelMatrix, glm::vec3(-4.f, 3.0f, 1.0f));
	addEntity(metalBox);

	Core::Mesh* cubeMesh = RMI.mesh("Data/Models/cube.obj");
	Ptr<Dg::TexturedObject> cube = std::make_shared<Dg::TexturedObject>(cubeMesh);
	cube->m_modelMatrix = glm::scale(cube->m_modelMatrix, glm::vec3(0.5f));
	cube->m_modelMatrix = glm::translate(cube->m_modelMatrix, glm::vec3(0.f, 1.0f, 0.0f));
	addEntity(cube);
}