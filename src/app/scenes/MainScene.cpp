#include "MainScene.h"

#include <memory>

#include "dengine/shader/PhongShader.h"
#include "dengine/camera/OrbitCamera.h"
#include "dengine/lights/SunLight.h"
#include "dengine/entity/TexturedObject.h"
#include "dengine/entity/ColoredObject.h"
#include "dengine/shader/Shaders.h"

#include "dengine/component/Entity.h"

MainScene::MainScene() : Scene()
{
	m_orbitCamera = std::make_shared<Dg::OrbitCamera>();
	m_camera = m_orbitCamera;
}

struct Transform : public DgTest::Component
{
  public:
	~Transform()
	{
		int x = 3;
		LOG_INFO("Transform DESTROY");
	}
	Transform(Transform&& other)
	{
		// Move constr
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		LOG_INFO("Transform MOVE");
	}
	Transform(Transform& other)
	{
		// Copy constr
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		LOG_INFO("Transform COPY");
	}

	float x{0.f}, y{0.f}, z{0.f};
	Transform() = default;
	Transform(float x, float y, float z) : x(x), y(y), z(z) {}
};

void ecsTest()
{
	// ECS test
	DgTest::Entity entity;
	entity.registerComponent<Transform>();

	Transform test{1.2f, 3.1f, 11.3f};
	//	entity.addComponent(Transform());
	entity.addComponent(std::move(test));
	// entity.createComponent<Transform>(1.0f, 2.4f, 3.1f);

	Transform& t = entity.getComponent<Transform>();
	t.x = 99.4f;

	LOG_INFO("Transform: {}, {}, {}", t.x, t.y, t.z);

	Transform& t2 = entity.getComponent<Transform>();

	LOG_INFO("Transform: {}, {}, {}", t2.x, t2.y, t2.z);
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

	m_lighting->m_shadowSunLight.intensity = 0.8f;
	m_lighting->m_shadowSunLight.color = glm::vec3(0.93, 0.98, 1.0);
	m_lighting->m_shadowSunLight.direction = glm::vec3(-0.73, -0.64, -0.21);
	m_lighting->m_shadowSunLight.pos = glm::vec3(35, 30, 10);
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
	plane->m_modelMatrix = glm::rotate(plane->m_modelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
	plane->m_modelMatrix = glm::scale(plane->m_modelMatrix, glm::vec3(15.f));
	addEntity(plane);

	Dg::Mesh* cameraMesh = RMI.mesh("Data/Models/camera.gltf");
	Ptr<Dg::TexturedObject> camera =
	    std::make_shared<Dg::TexturedObject>(cameraMesh, Dg::Shaders::instance().getShaderPtr<Dg::PhongShader>());
	camera->m_modelMatrix = glm::translate(camera->m_modelMatrix, glm::vec3(3.f, 1.0f, -2.0f));
	addEntity(camera);

	Dg::Mesh* boxMesh = RMI.mesh("Data/Models/box_metal.gltf");
	Ptr<Dg::TexturedObject> metalBox = std::make_shared<Dg::TexturedObject>(boxMesh);
	metalBox->m_modelMatrix = glm::translate(metalBox->m_modelMatrix, glm::vec3(-4.f, 3.0f, 1.0f));
	addEntity(metalBox);

	Dg::Mesh* duckMesh = RMI.mesh("Data/Models/Duck.gltf");
	Ptr<Dg::TexturedObject> duck = std::make_shared<Dg::TexturedObject>(duckMesh);
	duck->m_modelMatrix = glm::translate(duck->m_modelMatrix, glm::vec3(4.f, 2.0f, 1.0f));
	duck->m_highlight = true;
	addEntity(duck);

	Dg::Mesh* cubeMesh = RMI.mesh("Data/Models/cube.obj");
	Ptr<Dg::TexturedObject> cube = std::make_shared<Dg::TexturedObject>(cubeMesh);
	cube->m_modelMatrix = glm::scale(cube->m_modelMatrix, glm::vec3(0.5f));
	cube->m_modelMatrix = glm::translate(cube->m_modelMatrix, glm::vec3(0.f, 1.0f, 0.0f));
	addEntity(cube);

	// Ecs test
	ecsTest();
}

void MainScene::draw(int width, int height, Dg::SceneRenderTarget& renderTarget, const Dg::DisplayOptions& displayOptions)
{
	Scene::draw(width, height, renderTarget, displayOptions);
}
