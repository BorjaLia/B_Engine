#pragma once

#include <string>

namespace Engine
{
	// Forward declarations to avoid heavy includes in the base component
	class Node;
	class RendererBase;

	/// @defgroup Components ECS Components
	/// @brief All attachable components that define entity behavior and data.
	///
	/// The base class for all entities attached to the Scene Graph.
	/// @ingroup Components
	class Component
	{
	public:
		virtual ~Component() = default;

		bool IsActive() const { return isActive; }
		void SetActive(bool active) { isActive = active; }

		bool HasStarted() const { return hasStarted; }
		void SetStarted(bool started) { hasStarted = started; }

		bool IsPendingDestruction() const { return pendingDestruction; }
		void Destroy();

		// Basic lifecycle of any component
		virtual void Initialize() {}
		virtual void Start() {}
		virtual void FixedUpdate(float fixedDeltaTime) { (void)fixedDeltaTime; }
		virtual void Update(float deltaTime) { (void)deltaTime; }
		virtual void Draw(RendererBase* renderer) { (void)renderer; }
		virtual void DebugDraw(RendererBase* renderer) { (void)renderer; }

		virtual std::string ToString() const { return "Component"; }

		void SetOwner(Node* node) { owner = node; }
		Node* GetOwner() const { return owner; }

	protected:
		Node* owner = nullptr; // Pointer to the node that "owns" this component

	private:
		bool isActive = true;
		bool hasStarted = false;
		bool pendingDestruction = false;
	};
}