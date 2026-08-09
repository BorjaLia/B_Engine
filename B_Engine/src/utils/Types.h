#pragma once

#include <vector>
#include <variant>
#include <string>
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"

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

		std::string ToString() const
		{
			return "[R:" + std::to_string(r) + " G:" + std::to_string(g) + " B:" + std::to_string(b) + " A:" + std::to_string(a) + "]";
		}
	};

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

#pragma region 3D Graphics
	struct BoundingBox3D
	{
		Vector3f min;
		Vector3f max;
	};

	/// Represents a single piece of geometry. 
	/// Owns the raw CPU data (for physics/custom OpenGL) and the GPU IDs.
	struct Mesh
	{
		unsigned int vaoId = 0;                  // OpenGL Vertex Array Object ID
		std::vector<unsigned int> vboIds;        // OpenGL Vertex Buffer Object IDs
		int vertexCount = 0;
		int triangleCount = 0;

		// Raw CPU Data
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> texcoords;
		std::vector<unsigned short> indices;
	};

	/// A complete 3D model owned by the Engine, containing pure graphical data and bounds
	struct Model
	{
		std::vector<Mesh> meshes;
		BoundingBox3D bounds;
	};
#pragma endregion

#pragma region Shapes & Math
	// 2D Shapes
	struct CircleShape { float radius = 1.0f; };
	struct RectangleShape { Vector2f size = { 1.0f, 1.0f }; };
	struct LineShape { Vector2f start = { 0.0f, 0.0f }; Vector2f end = { 1.0f, 1.0f }; };
	struct PolygonShape { std::vector<Vector2f> localVertices; };

	using Shape = std::variant<CircleShape, RectangleShape, LineShape, PolygonShape>;
	
	// 3D Shapes
	struct Line3DShape { Vector3f start = { 0.0f, 0.0f, 0.0f }; Vector3f end = { 0.0f, 0.0f, 1.0f }; };
	struct Cube3DShape { Vector3f size = { 1.0f, 1.0f, 1.0f }; };
	struct Path3DShape { std::vector<Vector3f> localVertices; bool closed = false; };

	using Shape3D = std::variant<Line3DShape, Cube3DShape, Path3DShape>;

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

	struct DebugRenderCommand3D
	{
		Shape3D shape;
		Vector3f position;
		Vector3f rotation;
		Color color;
	};

	/// Command for rendering an opaque 3D model.
	struct ModelRenderCommand
	{
		const Model* model;           // Pointer to avoid deep copies!
		Matrix4x4 transformMatrix;    // By value (64 bytes snapshot)
		Color tint;
	};

	/// Command for rendering a debug wireframe of a model.
	struct DebugModelCommand
	{
		const Model* model;           // Pointer to raw CPU geometry
		Matrix4x4 transformMatrix;    // By value (64 bytes snapshot)
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