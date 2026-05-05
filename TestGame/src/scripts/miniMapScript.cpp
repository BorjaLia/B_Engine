#include "miniMapScript.h"

#include "core/Application.h"

MiniMapScript::MiniMapScript(Engine::CameraComponent* cam, Engine::SpriteComponent* sprite)
{
	this->cam = cam;
	this->sprite = sprite;
}

void MiniMapScript::OnStart()
{
	rendTex = Engine::Application::Get().GetResourceManager()->CreateRenderTexture(sprite->GetTargetSize());
	cam->SetRenderTarget(rendTex);
}

void MiniMapScript::OnUpdate(float)
{
	//rendTex.texture = sprite->texture;
}