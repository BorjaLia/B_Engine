#pragma once
#pragma once

#include "scenes/Scene.h" 

class TestScene3D : public Engine::IScene
{
public:
    void Build(Engine::SceneBuilder& builder) override;
};