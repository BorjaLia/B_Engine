#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../graphics/RendererBase.h"
#include "../core/interfaces/IAudio.h"
#include "../utils/Types.h"
#include "Font.h"
#include "../debug/MemoryTracker.h"

namespace Engine
{
    /// @defgroup Resources Resource Management
    /// @brief Texture, Font, and Audio caching.
    ///
    /// Centralized cache and factory for loading game assets (textures, fonts, audio).
    /// @ingroup Resources
    class ResourceManager
    {
    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        /// @param renderer Renderer interface used to load GPU-side resources.
        /// @param audio Audio interface used to load sound data (may be null if audio is disabled).
        ResourceManager(RendererBase* renderer, IAudio* audio = nullptr);
        ~ResourceManager();

        /// Returns a cached texture, loading it on first request. Falls back to a default texture if not found.
        Texture2D GetTexture(const std::string& filepath);

        /// Returns a 1x1 white pixel texture (used internally as a fallback and for solid shapes).
        Texture2D GetWhitePixel();

        /// Creates and tracks a render texture at the given pixel size.
        RenderTexture2D CreateRenderTexture(const Vector2i& size);

        /// Returns a cached font atlas, loading it on first request.
        Font* GetFont(const std::string& filepath, int fontSize);

        /// Returns a cached audio clip, loading it on first request.
        /// Returns an invalid clip (id == 0) if audio is not available.
        AudioClip GetAudioClip(const std::string& filepath);

        /// Returns a cached 3D Model, loading it on first request.
        /// Returns an invalid model (id == 0) if it fails to load.
        Model GetModel(const std::string& filepath);

        /// Unloads all resources and releases GPU/audio memory.
        void Clear();

    private:
        RendererBase* renderer;
        IAudio* audio;

        std::unordered_map<std::string, Texture2D> textures;
        std::unordered_map<std::string, Font*>     fonts;
        std::unordered_map<std::string, AudioClip> audioClips;
        std::unordered_map<std::string, Model>     models;

        std::vector<RenderTexture2D>               renderTextures;
    };
}