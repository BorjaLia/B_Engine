#include "RendererBase.h"
#include "../core/Application.h"
#include "../events/ApplicationEvents.h"

#include <algorithm>

namespace Engine
{
    bool RendererBase::Initialize()
    {
        resizeEventId = Application::Get().GetEventBus().Subscribe<WindowResizeEvent>(
            [this](WindowResizeEvent& e) { this->OnWindowResize(e); }
        );

        replayEventId = Application::Get().GetEventBus().Subscribe<ReplayStateEvent>(
            [this](ReplayStateEvent& e) { this->OnReplayStateChanged(e); }
        );

        return OnInit();
    }

    void RendererBase::Shutdown()
    {
        OnShutdown();

        Application::Get().GetEventBus().Unsubscribe(WindowResizeEvent::GetStaticType(), resizeEventId);
    }

    Vector2f RendererBase::ScreenToLogical(const Vector2f& screenPos) const
    {
        if (!useGlobalCanvas) return Vector2f(screenPos.x, -screenPos.y);

        float realX = screenPos.x;
        float realY = -screenPos.y;

        float logX = (realX - lbOffset.x) / lbScale;
        float logY = (realY - lbOffset.y) / lbScale;

        return Vector2f(logX, -logY);
    }

    void RendererBase::SetLogicalResolution(const Vector2i& size)
    {
        logicalSize = size;

        if (isReplaying)
        {
            if (globalCanvas.id != 0) UnloadRenderTexture(globalCanvas);
            globalCanvas = CreateRenderTexture(size);
            CalculateLetterbox();
        }
    }

    void RendererBase::CalculateLetterbox()
    {
        if (logicalSize.x == 0 || logicalSize.y == 0) return;

        if (!isReplaying || windowSize.x == logicalSize.x && windowSize.y == logicalSize.y)
        {
            lbScale = 1.0f;
            lbOffset = { 0.0f, 0.0f };
            useGlobalCanvas = false;
            return;
        }

        float scaleX = static_cast<float>(windowSize.x) / logicalSize.x;
        float scaleY = static_cast<float>(windowSize.y) / logicalSize.y;

        lbScale = std::min(scaleX, scaleY);
        lbOffset.x = (windowSize.x - (logicalSize.x * lbScale)) * 0.5f;
        lbOffset.y = (windowSize.y - (logicalSize.y * lbScale)) * 0.5f;

        useGlobalCanvas = true;
    }

    void RendererBase::OnWindowResize(WindowResizeEvent& e)
    {
        windowSize = e.GetSize();

        if (isReplaying)
        {
            CalculateLetterbox();
        }
        else
        {
            SetLogicalResolution(windowSize);
        }
    }
    void RendererBase::OnReplayStateChanged(ReplayStateEvent& e)
    {
        isReplaying = e.IsPlaying();

        if (isReplaying)
        {
            if (globalCanvas.id != 0) UnloadRenderTexture(globalCanvas);
            globalCanvas = CreateRenderTexture(logicalSize);
            CalculateLetterbox();
        }
        else
        {
            useGlobalCanvas = false;
            if (globalCanvas.id != 0)
            {
                UnloadRenderTexture(globalCanvas);
                globalCanvas.id = 0;
            }
            logicalSize = windowSize;
        }
    }
}