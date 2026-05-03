#include "UIAnchorComponent.h"

#include <sstream>

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "SpriteComponent.h"
#include "ButtonComponent.h"
#include "../graphics/RendererBase.h" 

namespace Engine
{
    UIAnchorComponent::UIAnchorComponent(AnchorPreset preset, Vector2f offset, Vector2f size)
        : preset(preset), customAnchor(0.0f, 0.0f), offset(offset), size(size), isDirty(true)
    {
        resizeEventId = Application::Get().GetEventBus().Subscribe<WindowResizeEvent>(
            [this](WindowResizeEvent& e) { this->OnWindowResize(e); }
        );
    }

    UIAnchorComponent::~UIAnchorComponent()
    {
        Application::Get().GetEventBus().Unsubscribe(WindowResizeEvent::GetStaticType(), resizeEventId);
    }

    Vector2f UIAnchorComponent::GetAnchorVector() const
    {
        switch (preset)
        {
        case AnchorPreset::TopLeft:      return { 0.0f, 0.0f };
        case AnchorPreset::TopCenter:    return { 0.5f, 0.0f };
        case AnchorPreset::TopRight:     return { 1.0f, 0.0f };
        case AnchorPreset::MiddleLeft:   return { 0.0f, 0.5f };
        case AnchorPreset::Center:       return { 0.5f, 0.5f };
        case AnchorPreset::MiddleRight:  return { 1.0f, 0.5f };
        case AnchorPreset::BottomLeft:   return { 0.0f, 1.0f };
        case AnchorPreset::BottomCenter: return { 0.5f, 1.0f };
        case AnchorPreset::BottomRight:  return { 1.0f, 1.0f };
        case AnchorPreset::Custom:       return customAnchor;
        default:                         return { 0.0f, 0.0f };
        }
    }

    void UIAnchorComponent::OnWindowResize(WindowResizeEvent& /*e*/)
    {
        isDirty = true;
    }

    void UIAnchorComponent::RecalculatePosition()
    {
        if (owner == nullptr || owner->transform == nullptr) return;

        float refWidth = static_cast<float>(Application::Get().GetWindow()->GetSize().x);
        float refHeight = static_cast<float>(Application::Get().GetWindow()->GetSize().y);
        Vector2f refGlobalPos(0.0f, 0.0f);

        if (owner->GetParent() != nullptr)
        {
            if (auto* parentUI = owner->GetParent()->GetComponent<UIAnchorComponent>())
            {
                refWidth = parentUI->GetSize().x;
                refHeight = parentUI->GetSize().y;
                refGlobalPos = owner->GetParent()->GetGlobalPosition();
            }
        }

        Vector2f anchor = GetAnchorVector();
        Vector2f finalGlobalPos(
            refGlobalPos.x + (refWidth * anchor.x) + offset.x,
            refGlobalPos.y + (refHeight * anchor.y) + offset.y
        );

        Vector2f parentGlobal = (owner->GetParent() != nullptr)
            ? owner->GetParent()->GetGlobalPosition()
            : Vector2f(0.0f, 0.0f);

        owner->transform->SetPosition(Vector2f(
            finalGlobalPos.x - parentGlobal.x,
            finalGlobalPos.y - parentGlobal.y
        ));

        // Ensure child transforms use this updated value immediately
        owner->transform->UpdateTransform();
    }

    void UIAnchorComponent::Start()
    {
        if (owner)
        {
            if (auto* sprite = owner->GetComponent<SpriteComponent>())
                sprite->SetLayer(RenderLayer::UI);
            if (auto* btn = owner->GetComponent<ButtonComponent>())
                btn->SetLayer(RenderLayer::UI);
        }

        RecalculatePosition();
        isDirty = false;
    }

    void UIAnchorComponent::Update(float /*dt*/)
    {
        // Lightweight update: only trigger math if dirtied by an event or hierarchy change
        if (!isDirty) return;

        RecalculatePosition();
        isDirty = false;
    }

    void UIAnchorComponent::MarkDirty()
    {
        isDirty = true;

        if (owner == nullptr) return;

        for (const auto& child : owner->GetChildren())
        {
            if (auto* childUI = child->GetComponent<UIAnchorComponent>())
                childUI->MarkDirty();
        }
    }

    std::string UIAnchorComponent::ToString() const
    {
        std::stringstream ss;
        ss << "UIAnchorComponent [Preset: " << static_cast<int>(preset)
            << " | Offset: " << offset << " | Size: " << size << "]";
        return ss.str();
    }
}