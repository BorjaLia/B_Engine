#include "miniMapScript.h"

#include "core/Application.h"

MiniMapScript::MiniMapScript(Engine::CameraComponent* cam, Engine::SpriteComponent* sprite)
{
	this->cam = cam;
	this->sprite = sprite;
}

void MiniMapScript::OnStart()
{
	if (!owner) return;
	ENGINE_LOG("Started minimap script");

	rendTex = Engine::Application::Get().GetResourceManager()->CreateRenderTexture(sprite->GetTargetSize());
	cam->SetRenderTarget(rendTex);

	sprite->SetTexture(rendTex.texture);
	sprite->SetFlipY(true);
}

void MiniMapScript::OnUpdate(float)
{
	//rendTex.texture = sprite->texture;
}