#pragma once

#include <vector>
#include <variant>
#include <iostream>
#include "../math/Vector2.h"
#include "../math/Vector3.h"

namespace Engine
{
#pragma region Graphics & UI
	/// @ingroup Utils
	struct Color
	{
		unsigned char r, g, b, a;

		constexpr Color() : r(255), g(255), b(255), a(255) {}

		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
			: r(r), g(g), b(b), a(a)
		{
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Color& c)
	{
		return os << "[R:" << static_cast<int>(c.r) << " G:" << static_cast<int>(c.g) << " B:" << static_cast<int>(c.b) << " A:" << static_cast<int>(c.a) << "]";
	}

	enum class Pivot
	{
		TopLeft, TopCenter, TopRight,
		LeftCenter, Center, RightCenter,
		BottomLeft, BottomCenter, BottomRight
	};

	inline constexpr Vector2f GetPivotMultiplier(Pivot pivot)
	{
		switch (pivot)
		{
		case Pivot::TopLeft: return { 0.0f, 0.0f };
		case Pivot::TopCenter: return { 0.5f, 0.0f };
		case Pivot::TopRight: return { 1.0f, 0.0f };
		case Pivot::LeftCenter: return { 0.0f, 0.5f };
		case Pivot::Center: return { 0.5f, 0.5f };
		case Pivot::RightCenter: return { 1.0f, 0.5f };
		case Pivot::BottomLeft: return { 0.0f, 1.0f };
		case Pivot::BottomCenter: return { 0.5f, 1.0f };
		case Pivot::BottomRight: return { 1.0f, 1.0f };
		}
		return { 0.0f, 0.0f };
	}

	struct Texture2D
	{
		unsigned int id = 0;
		Vector2i size;
		int mipmaps = 1;
		int format = 0;
	};

	struct RenderTexture2D
	{
		unsigned int id = 0;
		Texture2D texture;
	};

	enum class RenderLayer { World, UI };
#pragma endregion

#pragma region Shapes & Math
	struct CircleShape { float radius = 1.0f; };
	struct RectangleShape { Vector2f size = { 1.0f, 1.0f }; };
	struct LineShape { Vector2f start = { 0.0f, 0.0f }; Vector2f end = { 1.0f, 1.0f }; };
	struct PolygonShape { std::vector<Vector2f> localVertices; };

	using Shape = std::variant<CircleShape, RectangleShape, LineShape, PolygonShape>;

	struct Rect { Vector2f pos; Vector2f size; };

	struct SpriteRenderCommand
	{
		Texture2D texture;
		Rect sourceRect;
		Vector3f position;
		float rotation = 0.0f;
		Vector2f scale;
		Pivot pivot;
		Color tint;
		bool flipX = false;
		bool flipY = false;
		bool useSourceRect = false;
	};

	struct DebugRenderCommand
	{
		Shape shape;
		Vector2f position;
		float rotation = 0.0f;
		Color color;
	};
#pragma endregion

#pragma region Audio
	/// Opaque handle to a loaded sound. Cheap to copy; the backend owns the data.
	struct AudioClip
	{
		unsigned int id = 0;
		float defaultVolume = 1.0f;
		float defaultPitch = 1.0f;
	};

	/// Playback parameters supplied when triggering a sound.
	struct AudioPlayParams
	{
		float volume = 1.0f;  // 0.0 - 1.0
		float pitch = 1.0f;  // 1.0 = original speed
		bool loop = false;
	};
#pragma endregion
}