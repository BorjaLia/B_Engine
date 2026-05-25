#include "miniMapScript.h"

#include "core/Application.h"

MiniMapScript::MiniMapScript(Engine::CameraComponent* cam, Engine::SpriteComponent* mapSprite, Engine::SpriteComponent* borderSprite, Engine::ButtonComponent* resizeBtn, float minSize, float maxSize)
	: cam(cam), mapSprite(mapSprite), borderSprite(borderSprite), resizeBtn(resizeBtn), minSize(minSize), maxSize(maxSize)
{
}

void MiniMapScript::OnStart()
{
	if (!owner) return;
	ENGINE_LOG("Started minimap script");

	currentSize = borderSprite->GetTargetSize().x;

	rendTex = Engine::Application::Get().GetResourceManager()->CreateRenderTexture(mapSprite->GetTargetSize());
	cam->SetRenderTarget(rendTex);

	mapSprite->SetTexture(rendTex.texture);
	mapSprite->SetFlipY(true);

    if (resizeBtn)
    {
        resizeBtn->SetOnPress([this]()
            {
                this->isDragging = true;
                auto& input = Engine::Application::Get().GetInputManager();
                this->lastMousePos = {
                    input.GetAxis(Engine::Hash::GetHash("Pointer_X")),
                    input.GetAxis(Engine::Hash::GetHash("Pointer_Y"))
                };
            });

        resizeBtn->SetOnRelease([this]()
            {
                this->isDragging = false;
            });
    }
}

void MiniMapScript::OnUpdate(float /*deltaTime*/)
{
    if (isDragging)
    {
        auto& input = Engine::Application::Get().GetInputManager();

        Engine::Vector2f mousePos = {
            input.GetAxis(Engine::Hash::GetHash("Pointer_X")),
            input.GetAxis(Engine::Hash::GetHash("Pointer_Y"))
        };

        Engine::Vector2f delta = mousePos - lastMousePos;
        float deltaSize = (delta.x - delta.y) * 0.5f;

        if (deltaSize != 0.0f)
        {
            currentSize += deltaSize;

            if (currentSize < minSize) currentSize = minSize;
            if (currentSize > maxSize) currentSize = maxSize;

            borderSprite->SetTargetSize({ currentSize, currentSize });
            mapSprite->SetTargetSize({ currentSize - 20.0f, currentSize - 20.0f });
            resizeBtn->GetOwner()->transform.SetPosition(Engine::Vector2(currentSize, currentSize ));

            lastMousePos = mousePos;
        }
    }
}