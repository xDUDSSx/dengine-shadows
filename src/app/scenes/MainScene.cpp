#include "MainScene.h"

#include <memory>

#include "dengine/shader/PhongShader.h"
#include "dengine/camera/OrbitCamera.h"
#include "dengine/lights/SunLight.h"
#include "dengine/entity/TexturedObject.h"
#include "dengine/entity/ColoredObject.h"
#include "dengine/shader/Shaders.h"
#include "dengine/util/Math.h"

#include "dengine/component/Entity.h"

MainScene::MainScene() : Scene()
{
	m_orbitCamera = std::make_shared<Dg::OrbitCamera>();
	m_camera = m_orbitCamera;

	m_orbitCamera2 = std::make_shared<Dg::OrbitCamera>();
}

void randomGenerateScifiBlocks(Dg::Scene& scene, glm::vec3 origin)
{
	int range = 10;
	int minRangeX = 2;
	int minRangeZ = 4;
	for (int x = -range; x < range; x++)
	{
		for (int y = -range; y < range; y++)
		{
			if (abs(x) < minRangeX && abs(y) < minRangeZ)
				continue;
			if (Math::randomFloat(0.f, 1.f) > 0.1f)
				continue;
//			Dg::Mesh* buildingMesh =
//			    RMI.mesh(std::string("Data/Models/scifiblock") + std::to_string(Math::randomInt(1, 3)) + ".gltf");
			Dg::Mesh* buildingMesh = RMI.mesh(std::string("Data/Models/House.glb"));
			Ptr<Dg::TexturedObject> building = std::make_shared<Dg::TexturedObject>(buildingMesh);
			building->m_modelMatrix = glm::translate(building->m_modelMatrix, origin + glm::vec3(x * 20.f, 0.05f, y * 20.0f));
			building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(10.0f));
			building->m_modelMatrix =
			    glm::rotate(building->m_modelMatrix, glm::radians(Math::randomFloat(0.f, 180.f)), glm::vec3(0, 1, 0));

			//			building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(0.5f));
			//			building->m_modelMatrix = glm::translate(building->m_modelMatrix, glm::vec3(-60.f, -3.0f, -40.0f));
			scene.addEntity(building);
		}
	}
}

void MainScene::init()
{
	Scene::init();

	srand(4);

	m_orbitCamera->setFov(90.f);
	m_orbitCamera->setRotationX(60);
	m_orbitCamera->setZFar(360.0f);
	m_orbitCamera->setZNear(1.5f);
	m_orbitCamera->setZoomSpeed(m_orbitCamera->getZoomSpeed() * m_orbitCamera->getZNear() / 0.2f);
	m_orbitCamera->setTranslateSpeed(m_orbitCamera->getTranslateSpeed() * m_orbitCamera->getZNear() / 0.2f);

//	m_orbitCamera->setZNear(1.5f);
//	m_orbitCamera->setZoomSpeed(m_orbitCamera->getZoomSpeed() * m_orbitCamera->getZNear() / 0.2f);
//	m_orbitCamera->setTranslateSpeed(m_orbitCamera->getTranslateSpeed() * m_orbitCamera->getZNear() / 0.2f);
	m_orbitCamera2->setZFar(10.0f);
	m_orbitCamera2->setZFar(3000.0f);

	// Lights
	Dg::SunLight* sun = new Dg::SunLight();
	sun->intensity = 0.8f;
	sun->color = glm::vec3(0.93, 0.98, 1.0);
	sun->direction = glm::vec3(-0.73, -0.64, -0.21);
	sun->pos = glm::vec3(0, 4, 0);
	m_lighting->addLight(sun);

	m_lighting->m_shadowSunLight.intensity = 0.8f;
	m_lighting->m_shadowSunLight.color = glm::vec3(0.93, 0.98, 1.0);
	m_lighting->m_shadowSunLight.direction = glm::vec3(-0.73, -0.64, -0.21);
	m_lighting->m_shadowSunLight.pos = glm::vec3(90, 300, 60);
	m_lighting->m_shadowSunLight.updateShadowVolume(50, 1.0f, 100.0f);

	//	Dg::SunLight* sun2 = new Dg::SunLight();
	//	sun2->intensity = 0.08f;
	//	sun2->color = glm::vec3(0.804, 0.945, 1);
	//	sun2->specular = glm::vec3(0.0f); // Disable specular
	//	sun2->direction = glm::vec3(0.76, 0.58, 0.19);
	//	sun->pos = glm::vec3(0, 2, 0);
	//	m_lighting->addLight(sun2);

	Dg::Mesh* planeMesh = RMI.mesh("Data/Models/plane.gltf");
	Ptr<Dg::TexturedObject> plane =
	    std::make_shared<Dg::TexturedObject>(planeMesh, Dg::Shaders::instance().getShaderPtr<Dg::PhongShader>());
	plane->m_modelMatrix = glm::scale(plane->m_modelMatrix, glm::vec3(200.f, 1.0f, 200.f));
	plane->m_modelMatrix = glm::rotate(plane->m_modelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
	//	plane->m_modelMatrix = glm::scale(plane->m_modelMatrix, glm::vec3(50.f));
	//	plane->m_modelMatrix = glm::scale(plane->m_modelMatrix, glm::vec3(15.f));
	plane->m_shadowCaster = false;
	addEntity(plane);

	Dg::Mesh* cameraMesh = RMI.mesh("Data/Models/camera.gltf");
	Ptr<Dg::TexturedObject> camera =
	    std::make_shared<Dg::TexturedObject>(cameraMesh, Dg::Shaders::instance().getShaderPtr<Dg::PhongShader>());
	camera->m_modelMatrix = glm::translate(camera->m_modelMatrix, glm::vec3(3.f, 1.0f, -2.0f));
	addEntity(camera);

	{
		Dg::Mesh* boxMesh = RMI.mesh("Data/Models/box_metal.gltf");
		Ptr<Dg::TexturedObject> metalBox = std::make_shared<Dg::TexturedObject>(boxMesh);
		metalBox->m_modelMatrix = glm::translate(metalBox->m_modelMatrix, glm::vec3(-4.f, 4.0f, 1.0f));
		addEntity(metalBox);
	}

	Dg::Mesh* duckMesh = RMI.mesh("Data/Models/Duck.gltf");
	Ptr<Dg::TexturedObject> duck = std::make_shared<Dg::TexturedObject>(duckMesh);
	duck->m_modelMatrix = glm::translate(duck->m_modelMatrix, glm::vec3(4.f, 2.5f, 1.0f));
	duck->m_modelMatrix = glm::rotate(duck->m_modelMatrix, glm::radians(34.f), glm::vec3(0.f, 1.f, 0.f));
	duck->m_modelMatrix = glm::scale(duck->m_modelMatrix, glm::vec3(2.0f));
	addEntity(duck);

	Dg::Mesh* cubeMesh = RMI.mesh("Data/Models/cube.obj");
	Ptr<Dg::TexturedObject> cube = std::make_shared<Dg::TexturedObject>(cubeMesh);
	cube->m_modelMatrix = glm::scale(cube->m_modelMatrix, glm::vec3(0.5f));
	cube->m_modelMatrix = glm::translate(cube->m_modelMatrix, glm::vec3(10.f, 1.0f, 10.0f));
	for (auto& meshPart : cube->m_mesh->m_meshParts)
	{
		meshPart.material.ambient = glm::vec3(0.1f);
	}
	addEntity(cube);

	{
		Dg::Mesh* buildingMesh = RMI.mesh("Data/Models/buildingBase.glb");
		Ptr<Dg::TexturedObject> building = std::make_shared<Dg::TexturedObject>(buildingMesh);
		building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(0.5f));
		building->m_modelMatrix = glm::translate(building->m_modelMatrix, glm::vec3(-60.f, -3.0f, -40.0f));
		addEntity(building);
	}
	{
		Dg::Mesh* buildingMesh = RMI.mesh("Data/Models/buildingSmall.glb");
		Ptr<Dg::TexturedObject> building = std::make_shared<Dg::TexturedObject>(buildingMesh);
		building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(0.5f));
		building->m_modelMatrix = glm::translate(building->m_modelMatrix, glm::vec3(-60.f, -3.0f, -40.0f));
		addEntity(building);
	}
	{
		Dg::Mesh* buildingMesh = RMI.mesh("Data/Models/buildingLarge.glb");
		Ptr<Dg::TexturedObject> building = std::make_shared<Dg::TexturedObject>(buildingMesh);
		building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(0.5f));
		building->m_modelMatrix = glm::translate(building->m_modelMatrix, glm::vec3(-60.f, -3.0f, -40.0f));
		addEntity(building);
	}
	{
		Dg::Mesh* buildingMesh = RMI.mesh("Data/Models/hut.obj");
		Ptr<Dg::TexturedObject> building = std::make_shared<Dg::TexturedObject>(buildingMesh);
		building->m_modelMatrix = glm::translate(building->m_modelMatrix, glm::vec3(-40.f, -0.1f, 35.0f));
		building->m_modelMatrix = glm::scale(building->m_modelMatrix, glm::vec3(0.5f));
		for (auto& meshPart : building->m_mesh->m_meshParts)
		{
			meshPart.material.ambient = glm::vec3(0.085f);
		}
		addEntity(building);
	}

	int range = 10;
	int minRange = 2;
	for (int x = -range; x < range; x++)
	{
		if (abs(x) < minRange)
			continue;
		Dg::Mesh* boxMesh = RMI.mesh("Data/Models/Duck.gltf");
		Ptr<Dg::TexturedObject> metalBox = std::make_shared<Dg::TexturedObject>(boxMesh);
		metalBox->m_modelMatrix = glm::translate(
		    metalBox->m_modelMatrix, glm::vec3(Math::randomFloat(-5.0f, 5.0f), Math::randomFloat(0.0f, 30.0f), x * 6.f));
		metalBox->m_modelMatrix = glm::scale(metalBox->m_modelMatrix, glm::vec3(2.0f));
		addEntity(metalBox);
	}

	randomGenerateScifiBlocks(*this, glm::vec3(0, 0, 0));

	//	int range = 10;
	//	int minRange = 2;
	//	for (int x = -range; x < range; x++)
	//	{
	//		for (int y = -range; y < range; y++)
	//		{
	//			if (abs(x) < minRange || abs(y) < minRange)
	//				continue;
	//			Dg::Mesh* boxMesh = RMI.mesh("Data/Models/box_metal.gltf");
	//			Ptr<Dg::TexturedObject> metalBox = std::make_shared<Dg::TexturedObject>(boxMesh);
	//			metalBox->m_modelMatrix = glm::translate(metalBox->m_modelMatrix, glm::vec3(x * 6.f,
	// Math::randomFloat(0.0f, 10.0f),
	// y
	//* 6.0f)); 			addEntity(metalBox);
	//		}
	//	}

	// DONT FORGET THIS !!!!!!!!!!!!!!!!!!!!!
	precalculateBoundingBoxes(); // TODO: Objects should handle their boxes on their own, this is a temporary workaround
}

void MainScene::draw(int width, int height, Dg::SceneRenderTarget& renderTarget, const Dg::DisplayOptions& displayOptions)
{
	Scene::draw(width, height, renderTarget, displayOptions);
}

void MainScene::draw(int width, int height, glm::mat4 view, glm::mat4 projection, Dg::SceneRenderTarget& renderTarget,
                     const Dg::DisplayOptions& displayOptions)
{
	Scene::draw(width, height, view, projection, renderTarget, displayOptions);
}
