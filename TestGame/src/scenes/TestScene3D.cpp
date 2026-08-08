#include "TestScene3D.h"

#include <cmath>

#include "core/Application.h"
#include "scenes/SceneManager.h"
#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"
#include "components/ScriptComponent.h"
#include "components/MeshRendererComponent.h"
#include "components/TextComponent.h"
#include "components/UIAnchorComponent.h"
#include "components/PlayerMovement3DComponent.h"

#include "utils/StringHash.h" 
#include "math/MathUtils.h"

#include "../scripts/CameraLookScript.h"

#include <sstream>
#include <iomanip>

void CreatePlayerDebugUI(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Node* playerNode);
void CreateCameraDebugUI(Engine::SceneBuilder& builder, Engine::Font* font, Engine::CameraComponent* camera);

class CameraFollow3DScript : public Engine::Script
{
public:
	Engine::Node* targetNode;
	float distance;
	float height;
	float currentYaw = 0.0f;
	Engine::CameraComponent* cam = nullptr;

	CameraFollow3DScript(Engine::Node* target, float dist, float h)
		: targetNode(target), distance(dist), height(h)
	{
	}

	void OnStart() override
	{
		if (owner) cam = owner->GetComponent<Engine::CameraComponent>();
	}

	void OnUpdate(float /*deltaTime*/) override
	{
		//if (!targetNode || !cam) return;

		//auto& mapper = Engine::Application::Get().GetInputManager();

		//float rotXInput = mapper.GetAxis(Engine::Hash::GetHash("Game_RotX"));
		//float rotYInput = mapper.GetAxis(Engine::Hash::GetHash("Game_RotY"));

		//float rotationSpeed = 2.0f;
		//currentYaw -= rotXInput * rotationSpeed * deltaTime;
		//height += rotYInput * deltaTime;

		//Engine::Vector3f targetPos = targetNode->transform.GetGlobalPosition();

		//Engine::Vector3f newCamPos;
		//newCamPos.x = targetPos.x + std::sin(currentYaw) * distance;
		//newCamPos.y = targetPos.y + height;
		//newCamPos.z = targetPos.z + std::cos(currentYaw) * distance;

		//owner->transform.SetPosition(newCamPos);

		//owner->transform.SetEulerAngles({ 0.0f, currentYaw + Engine::PI, 0.0f });

		//cam->SetTarget(targetPos);
	}
};

void TestScene3D::Build(Engine::SceneBuilder& builder)
{
	auto& app = Engine::Application::Get();
	auto& rm = *app.GetResourceManager();

	Engine::Texture2D texPlayer = rm.GetTexture("res/sprites/enemy.png");
	Engine::Texture2D texTree = rm.GetTexture("res/sprites/box.png");

	Engine::Node* playerNode = builder.CreateNode("Player3D");
	playerNode->transform.SetPosition({ 0.0f, 0.0f, 0.0f });

	//playerNode->AddComponent<Engine::SpriteComponent>(texPlayer, Engine::Pivot::BottomCenter);

	Engine::Node* camNode = builder.CreateChildNode(playerNode,"Camera3D");

	auto* cam = camNode->AddComponent<Engine::CameraComponent>();

	playerNode->AddComponent<Engine::PlayerMovement3DComponent>(cam, 200.0f);

	cam->SetProjectionType(Engine::CameraProjection::Perspective);
	cam->SetFOV(60.0f);
	cam->SetUpVector({ 0.0f, 1.0f, 0.0f });

	//camNode->AddComponent<Engine::ScriptComponent>(new CameraFollow3DScript(playerNode, 250.0f, 100.0f));
	camNode->AddComponent<Engine::ScriptComponent>(new CameraLookScript(0.25f));

	// 3. ENTORNO 3D 
	for (int i = 0; i < 40; ++i)
	{
		Engine::Node* treeNode = builder.CreateNode("Tree_" + std::to_string(i));

		float randX = (rand() % 2000) - 1000.0f;
		float randZ = (rand() % 2000) - 1000.0f;

		treeNode->transform.SetPosition({ randX, 0.0f, randZ });
		treeNode->AddComponent<Engine::SpriteComponent>(texTree, Engine::Pivot::BottomCenter);
	}

	// 1. Cargar el modelo a través de nuestro nuevo ResourceManager
	Engine::Model myCubeModel = rm.GetModel("res/models/cube.obj");

	// 2. Crear el Nodo y el Componente
	Engine::Node* cubeNode = builder.CreateNode("MyCube");
	cubeNode->AddComponent<Engine::MeshRendererComponent>(myCubeModel);

	cubeNode->transform.SetScale(Engine::Vector3f(10.0f, 10.0f, 10.0f));

	// 4. UI DE DEBUG
	Engine::Font* font = rm.GetFont("res/fonts/ReemKufiFunRegular.ttf", 24);
	CreatePlayerDebugUI(builder, font, playerNode);
	CreateCameraDebugUI(builder, font, cam);
}

class PlayerDebugUIScript : public Engine::Script
{
public:
	Engine::Node* targetPlayer;
	Engine::TextComponent* textComp = nullptr;
	Engine::PlayerMovement3DComponent* movementComp = nullptr;

	PlayerDebugUIScript(Engine::Node* player) : targetPlayer(player) {}

	void OnStart() override
	{
		if (owner) textComp = owner->GetComponent<Engine::TextComponent>();
		if (targetPlayer) movementComp = targetPlayer->GetComponent<Engine::PlayerMovement3DComponent>();
	}

	void OnUpdate(float /*deltaTime*/) override
	{
		if (!textComp || !movementComp || !targetPlayer) return;

		Engine::Vector3f pos = targetPlayer->transform.GetGlobalPosition();
		Engine::Vector2f input = movementComp->GetCurrentInput();
		Engine::Vector3f vel = movementComp->GetCurrentVelocity();

		std::stringstream ss;
		// Fijamos la precisión para que el texto no baile cuando haya muchos decimales
		ss << std::fixed << std::setprecision(2);
		ss << "--- PLAYER 3D DEBUG ---\n";
		ss << "Pos:    X:" << pos.x << " Y:" << pos.y << " Z:" << pos.z << "\n";
		ss << "Input:  X:" << input.x << " Y:" << input.y << "\n";
		ss << "Vel:    X:" << vel.x << " Y:" << vel.y << " Z:" << vel.z << "\n";
		ss << "Speed:  " << movementComp->GetSpeed();

		textComp->text = ss.str();
	}
};

void CreatePlayerDebugUI(Engine::SceneBuilder& builder, Engine::Font* font, Engine::Node* playerNode)
{
	Engine::Node* node = builder.CreateNode("PlayerDebugUI");

	node->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::TopLeft, Engine::Vector2f(20.0f, 20.0f));

	auto* txt = node->AddComponent<Engine::TextComponent>(
		font, "Cargando debug...", 24.0f,
		Engine::Color{ 255, 255, 255, 255 },
		Engine::RenderLayer::UI
	);
	txt->alignment = Engine::TextAlignment::Left;
	txt->pivot = Engine::Pivot::TopLeft;

	node->AddComponent<Engine::ScriptComponent>(new PlayerDebugUIScript(playerNode));
}

class CameraDebugUIScript : public Engine::Script
{
public:
	Engine::CameraComponent* targetCamera;
	Engine::TextComponent* textComp = nullptr;

	CameraDebugUIScript(Engine::CameraComponent* camera) : targetCamera(camera) {}

	void OnStart() override
	{
		if (owner) textComp = owner->GetComponent<Engine::TextComponent>();
	}

	void OnUpdate(float /*deltaTime*/) override
	{
		if (!textComp || !targetCamera || !targetCamera->GetOwner()) return;

		Engine::Vector3f pos = targetCamera->GetOwner()->transform.GetGlobalPosition();
		Engine::Vector3f rot = targetCamera->GetOwner()->transform.GetEulerAngles();
		Engine::Vector3f target = targetCamera->GetTarget();

		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << "--- CAMERA 3D DEBUG ---\n";
		ss << "Pos:    X:" << pos.x << " Y:" << pos.y << " Z:" << pos.z << "\n";
		ss << "Rot:    X:" << rot.x << " Y:" << rot.y << " Z:" << rot.z << "\n";
		ss << "Target: X:" << target.x << " Y:" << target.y << " Z:" << target.z << "\n";
		ss << "FOV:    " << targetCamera->GetFOV();

		textComp->text = ss.str();
	}
};

void CreateCameraDebugUI(Engine::SceneBuilder& builder, Engine::Font* font, Engine::CameraComponent* camera)
{
	Engine::Node* node = builder.CreateNode("CameraDebugUI");

	node->AddComponent<Engine::UIAnchorComponent>(Engine::AnchorPreset::TopRight, Engine::Vector2f(-20.0f, 20.0f));

	auto* txt = node->AddComponent<Engine::TextComponent>(
		font, "Cargando cam debug...", 24.0f,
		Engine::Color{ 180, 255, 180, 255 },
		Engine::RenderLayer::UI
	);
	txt->alignment = Engine::TextAlignment::Right;
	txt->pivot = Engine::Pivot::TopRight;

	node->AddComponent<Engine::ScriptComponent>(new CameraDebugUIScript(camera));
}