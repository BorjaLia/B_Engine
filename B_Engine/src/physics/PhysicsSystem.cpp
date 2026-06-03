#include "PhysicsSystem.h"

#include <algorithm>

#include "ICollisionListener.h"
#include "../math/MathUtils.h"
#include "../math/Matrix3x3.h"
#include "../scenes/Node.h"
#include "../components/ColliderComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/TriggerAreaComponent.h"

namespace Engine
{
    // Anonymous namespace to keep internal physics helpers private to this file
    namespace
    {
        struct CollisionPair
        {
            PhysicsBody* bodyA;
            PhysicsBody* bodyB;
            Physics::CollisionManifold manifold;
        };

        void ResolveVelocity(CollisionPair& pair)
        {
            auto* bodyA = pair.bodyA->rigidBody;
            auto* bodyB = pair.bodyB->rigidBody;

            // Only Dynamic bodies receive impulses (invMass > 0).
            // Kinematic bodies contribute their velocity to relVel so they push Dynamic bodies,
            // but invMass = 0 means the solver never pushes them back.
            float invMassA = (bodyA && bodyA->GetType() == BodyType::Dynamic) ? 1.0f / bodyA->GetMass() : 0.0f;
            float invMassB = (bodyB && bodyB->GetType() == BodyType::Dynamic) ? 1.0f / bodyB->GetMass() : 0.0f;

            if (invMassA == 0.0f && invMassB == 0.0f) return;

            float invInertiaA = (bodyA && bodyA->GetType() == BodyType::Dynamic && !bodyA->GetLockRotation()) ? 1.0f / bodyA->GetInertia() : 0.0f;
            float invInertiaB = (bodyB && bodyB->GetType() == BodyType::Dynamic && !bodyB->GetLockRotation()) ? 1.0f / bodyB->GetInertia() : 0.0f;

            float restitution = std::min(bodyA ? bodyA->GetRestitution() : 0.0f, bodyB ? bodyB->GetRestitution() : 0.0f);
            float friction = std::sqrt((bodyA ? bodyA->GetFriction() : 0.5f) * (bodyB ? bodyB->GetFriction() : 0.5f));

            Vector2f centerOfMassA = pair.bodyA->collider->GetOwner()->transform.GetGlobalPosition();
            Vector2f centerOfMassB = pair.bodyB->collider->GetOwner()->transform.GetGlobalPosition();

            Vector2f totalLinearImpulseA(0, 0);
            float totalAngularImpulseA = 0.0f;
            Vector2f totalLinearImpulseB(0, 0);
            float totalAngularImpulseB = 0.0f;

            for (int i = 0; i < pair.manifold.contactCount; ++i)
            {
                Vector2f rA = pair.manifold.contactPoints[i] - centerOfMassA;
                Vector2f rB = pair.manifold.contactPoints[i] - centerOfMassB;

                // Kinematic velocity is included so a moving platform pushes objects
                Vector2f vA = bodyA ? bodyA->GetVelocity() : Vector2f(0, 0);
                Vector2f vB = bodyB ? bodyB->GetVelocity() : Vector2f(0, 0);
                float angVA = bodyA ? bodyA->GetAngularVelocity() : 0.0f;
                float angVB = bodyB ? bodyB->GetAngularVelocity() : 0.0f;

                Vector2f relVel = (vA + rA.Cross(angVA)) - (vB + rB.Cross(angVB));
                float velAlongNormal = relVel.Dot(pair.manifold.normal);

                if (velAlongNormal > 0) continue; // Already separating

                float rAnCross = rA.Cross(pair.manifold.normal);
                float rBnCross = rB.Cross(pair.manifold.normal);
                float invMassSum = invMassA + invMassB +
                    (rAnCross * rAnCross) * invInertiaA +
                    (rBnCross * rBnCross) * invInertiaB;

                // Normal (bounce) impulse
                float j = -(1.0f + restitution) * velAlongNormal;
                j /= invMassSum;
                j /= static_cast<float>(pair.manifold.contactCount);

                Vector2f impulse = pair.manifold.normal * j;

                if (invMassA > 0.0f)
                {
                    totalLinearImpulseA += impulse * invMassA;
                    if (!bodyA->GetLockRotation())
                        totalAngularImpulseA += rA.Cross(impulse) * invInertiaA;
                }
                if (invMassB > 0.0f)
                {
                    // Care: The impulse is subtracted from B to push it in the opposite direction
                    totalLinearImpulseB -= impulse * invMassB;
                    if (!bodyB->GetLockRotation())
                        totalAngularImpulseB -= rB.Cross(impulse) * invInertiaB;
                }

                // Friction impulse
                vA = bodyA ? bodyA->GetVelocity() : Vector2f(0, 0);
                vB = bodyB ? bodyB->GetVelocity() : Vector2f(0, 0);
                angVA = bodyA ? bodyA->GetAngularVelocity() : 0.0f;
                angVB = bodyB ? bodyB->GetAngularVelocity() : 0.0f;
                relVel = (vA + rA.Cross(angVA)) - (vB + rB.Cross(angVB));

                Vector2f tangent = relVel - (pair.manifold.normal * relVel.Dot(pair.manifold.normal));
                if (tangent.MagnitudeSquared() > 0.0001f)
                {
                    tangent.Normalize();
                    float jt = -relVel.Dot(tangent);

                    float rAtCross = rA.Cross(tangent);
                    float rBtCross = rB.Cross(tangent);
                    float invMassSumTangent = invMassA + invMassB +
                        (rAtCross * rAtCross) * invInertiaA +
                        (rBtCross * rBtCross) * invInertiaB;

                    jt /= invMassSumTangent;
                    jt /= static_cast<float>(pair.manifold.contactCount);

                    // Coulomb's law: friction impulse clamped to normal impulse magnitude
                    jt = std::max(-j * friction, std::min(jt, j * friction));
                    Vector2f frictionImpulse = tangent * jt;

                    // ACCUMULATE FRICTION
                    if (invMassA > 0.0f)
                    {
                        totalLinearImpulseA += frictionImpulse * invMassA;
                        if (!bodyA->GetLockRotation())
                            totalAngularImpulseA += rA.Cross(frictionImpulse) * invInertiaA;
                    }
                    if (invMassB > 0.0f)
                    {
                        totalLinearImpulseB -= frictionImpulse * invMassB;
                        if (!bodyB->GetLockRotation())
                            totalAngularImpulseB -= rB.Cross(frictionImpulse) * invInertiaB;
                    }
                }
            }

            if (bodyA && invMassA > 0.0f)
            {
                bodyA->SetVelocity(bodyA->GetVelocity() + totalLinearImpulseA);
                if (!bodyA->GetLockRotation())
                    bodyA->SetAngularVelocity(bodyA->GetAngularVelocity() + totalAngularImpulseA);
            }
            if (bodyB && invMassB > 0.0f)
            {
                // Remember that we accumulated the subtraction for B above, so we add the final accumulator here.
                bodyB->SetVelocity(bodyB->GetVelocity() + totalLinearImpulseB);
                if (!bodyB->GetLockRotation())
                    bodyB->SetAngularVelocity(bodyB->GetAngularVelocity() + totalAngularImpulseB);
            }
        }

        void ApplyPositionalCorrection(CollisionPair& pair)
        {
            auto* bodyA = pair.bodyA->rigidBody;
            auto* bodyB = pair.bodyB->rigidBody;

            // Only Dynamic bodies get nudged — Static and Kinematic stay put
            float invMassA = (bodyA && bodyA->GetType() == BodyType::Dynamic) ? 1.0f / bodyA->GetMass() : 0.0f;
            float invMassB = (bodyB && bodyB->GetType() == BodyType::Dynamic) ? 1.0f / bodyB->GetMass() : 0.0f;

            if (invMassA == 0.0f && invMassB == 0.0f) return;

            // percent: fraction of penetration corrected per step. 0.35 avoids jitter at rest.
            // slop: allowed penetration before correction fires. Prevents float noise jitter.
            const float percent = 0.35f;
            const float slop = 0.05f;

            Vector2f correction = pair.manifold.normal * (std::max(pair.manifold.depth - slop, 0.0f) / (invMassA + invMassB)) * percent;

            if (invMassA > 0.0f)
            {
                Vector2f currentPos = pair.bodyA->collider->GetOwner()->transform.GetPosition();
                pair.bodyA->collider->GetOwner()->transform.SetPosition(currentPos + (correction * invMassA));
            }

            if (invMassB > 0.0f)
            {
                Vector2f currentPos = pair.bodyB->collider->GetOwner()->transform.GetPosition();
                pair.bodyB->collider->GetOwner()->transform.SetPosition(currentPos - (correction * invMassB));
            }
        }

        void DispatchCollisionEvents(CollisionPair& pair)
        {
            for (const auto& comp : pair.bodyA->collider->GetOwner()->GetAllComponents())
            {
                if (auto* listener = dynamic_cast<ICollisionListener*>(comp.instance))
                {
                    listener->OnCollision(pair.manifold);
                }
            }

            Physics::CollisionManifold invertedHit = pair.manifold;
            invertedHit.normal = -pair.manifold.normal;

            for (const auto& comp : pair.bodyB->collider->GetOwner()->GetAllComponents())
            {
                if (auto* listener = dynamic_cast<ICollisionListener*>(comp.instance))
                {
                    listener->OnCollision(invertedHit);
                }
            }
        }
    } // End Anonymous Namespace

    PhysicsSystem::PhysicsSystem()
    {
        activeBodies.reserve(1000);
        obbCache.reserve(1000);
        activeTriggers.reserve(100);
    }

    void PhysicsSystem::Update(Node* rootScene, float fixedDeltaTime)
    {
        (void)fixedDeltaTime;
        if (!rootScene) return;

        PrePass();
        SolveCollisions();
        DetectTriggers();
    }

    void PhysicsSystem::RegisterCollider(ColliderComponent* collider)
    {
        PhysicsBody body;
        body.collider = collider;
        body.rigidBody = collider->GetOwner()->GetComponent<RigidBodyComponent>();

        body.isStatic = (body.rigidBody == nullptr || body.rigidBody->GetType() == BodyType::Static);
        body.needsObbUpdate = true;

        activeBodies.push_back(body);
        obbCache.push_back(Physics::OBB{});
    }

    void PhysicsSystem::UnregisterCollider(ColliderComponent* collider)
    {
        // Swap and Pop O(1) to maintain activeBodies and obbCache perfectly synchronized
        for (size_t i = 0; i < activeBodies.size(); ++i)
        {
            if (activeBodies[i].collider == collider)
            {
                activeBodies[i] = activeBodies.back();
                activeBodies.pop_back();

                obbCache[i] = obbCache.back();
                obbCache.pop_back();
                return;
            }
        }
    }

    void PhysicsSystem::RegisterTrigger(TriggerAreaComponent* trigger)
    {
        if (std::find(activeTriggers.begin(), activeTriggers.end(), trigger) == activeTriggers.end())
        {
            activeTriggers.push_back(trigger);
        }
    }

    void PhysicsSystem::UnregisterTrigger(TriggerAreaComponent* trigger)
    {
        activeTriggers.erase(std::remove(activeTriggers.begin(), activeTriggers.end(), trigger), activeTriggers.end());
    }

    void PhysicsSystem::OnRigidBodyAdded(RigidBodyComponent* rigidBody)
    {
        for (auto& body : activeBodies)
        {
            if (body.collider->GetOwner() == rigidBody->GetOwner())
            {
                body.rigidBody = rigidBody;
                body.isStatic = (rigidBody->GetType() == BodyType::Static);
                body.needsObbUpdate = true;
            }
        }
    }

    void PhysicsSystem::OnRigidBodyRemoved(RigidBodyComponent* rigidBody)
    {
        for (auto& body : activeBodies)
        {
            if (body.rigidBody == rigidBody)
            {
                body.rigidBody = nullptr;
                body.isStatic = true;
                body.needsObbUpdate = true;
            }
        }
    }

    void PhysicsSystem::MarkObbDirty(ColliderComponent* collider)
    {
        for (auto& body : activeBodies)
        {
            if (body.collider == collider)
            {
                body.needsObbUpdate = true;
                return;
            }
        }
    }

    void PhysicsSystem::PrePass()
    {
        for (size_t i = 0; i < activeBodies.size(); ++i)
        {
            auto& body = activeBodies[i];
            auto* col = body.collider;

            if (!col->IsActive() || !col->GetOwner()->IsActive()) continue;

            col->SetDebugColor({ 0, 255, 0, 255 });

            // Data-Oriented optimization: Skip heavy math if static and cached
            if (!body.isStatic || body.needsObbUpdate)
            {
                if (std::holds_alternative<RectangleShape>(col->GetShape()))
                {
                    const auto rect = std::get<RectangleShape>(col->GetShape());
                    float rot = col->GetOwner()->transform.GetRotation2D() * DEG2RAD;
                    Vector2f rotatedOffset = Matrix3x3::Rotation(rot) * col->GetOffset();
                    Vector2f globalPos = col->GetOwner()->transform.GetGlobalPosition();

                    Vector2f pos = globalPos + rotatedOffset;

                    obbCache[i] = Physics::GetOBB(pos, rect.size, rot);
                }
                else
                {
                    obbCache[i] = Physics::OBB{};
                }
                body.needsObbUpdate = false;
            }
        }
    }

    void PhysicsSystem::SolveCollisions()
    {
        std::vector<CollisionPair> contacts;
        contacts.reserve(100);

        size_t count = activeBodies.size();
        for (size_t i = 0; i < count; ++i)
        {
            auto& bodyA = activeBodies[i];
            if (!bodyA.collider->IsActive() || !bodyA.collider->GetOwner()->IsActive()) continue;
        
            for (size_t j = i + 1; j < count; ++j)
            {
                auto& bodyB = activeBodies[j];
                if (!bodyB.collider->IsActive() || !bodyB.collider->GetOwner()->IsActive()) continue;

                Shape shapeA = bodyA.collider->GetShape();
                Shape shapeB = bodyB.collider->GetShape();

                bool isRectA = std::holds_alternative<RectangleShape>(shapeA);
                bool isRectB = std::holds_alternative<RectangleShape>(shapeB);

                float rotA = bodyA.collider->GetOwner()->transform.GetRotation2D() * DEG2RAD;
                float rotB = bodyB.collider->GetOwner()->transform.GetRotation2D() * DEG2RAD;

                Vector2f globalPosA = bodyA.collider->GetOwner()->transform.GetGlobalPosition();
                Vector2f posA = globalPosA + (Matrix3x3::Rotation(rotA) * bodyA.collider->GetOffset());

                Vector2f globalPosB = bodyB.collider->GetOwner()->transform.GetGlobalPosition();
                Vector2f posB = globalPosB + (Matrix3x3::Rotation(rotB) * bodyB.collider->GetOffset());

                Physics::CollisionManifold manifold;

                if (isRectA && isRectB)
                    manifold = Physics::CheckCollision(obbCache[i], obbCache[j]);
                else if (!isRectA && !isRectB)
                    manifold = Physics::CheckCollision(posA, std::get<CircleShape>(shapeA).radius, posB, std::get<CircleShape>(shapeB).radius);
                else if (!isRectA && isRectB)
                    manifold = Physics::CheckCollision(posA, std::get<CircleShape>(shapeA).radius, obbCache[j]);
                else
                {
                    manifold = Physics::CheckCollision(posB, std::get<CircleShape>(shapeB).radius, obbCache[i]);
                    if (manifold.isColliding) manifold.normal = -manifold.normal;
                }

                if (manifold.isColliding)
                {
                    contacts.push_back({ &bodyA, &bodyB, manifold });
                    bodyA.collider->SetDebugColor({ 255, 0, 0, 255 });
                    bodyB.collider->SetDebugColor({ 255, 0, 0, 255 });
                }
            }
        }

        // Iterative velocity solver — more iterations = more stable stacks, more CPU cost
        for (int iter = 0; iter < solverIterations; ++iter)
        {
            for (auto& pair : contacts)
            {
                ResolveVelocity(pair);
            }
        }

        for (auto& pair : contacts)
        {
            ApplyPositionalCorrection(pair);
            DispatchCollisionEvents(pair);
        }
    }

    void PhysicsSystem::DetectTriggers()
    {
        for (auto* trigger : activeTriggers)
        {
            if (!trigger->IsActive() || !trigger->GetOwner()->IsActive()) continue;

            trigger->SetDebugColor({ 255, 255, 0, 150 });

            bool isRectT = std::holds_alternative<RectangleShape>(trigger->GetShape());
            float rotT = trigger->GetOwner()->transform.GetRotation2D() * DEG2RAD;

            Vector2f globalPosT = trigger->GetOwner()->transform.GetGlobalPosition();
            Vector2f posT = globalPosT + (Matrix3x3::Rotation(rotT) * trigger->GetOffset());

            Physics::OBB obbT;
            if (isRectT) obbT = Physics::GetOBB(posT, std::get<RectangleShape>(trigger->GetShape()).size, rotT);

            for (size_t i = 0; i < activeBodies.size(); ++i)
            {
                auto& body = activeBodies[i];
                if (!body.collider->IsActive() || !body.collider->GetOwner()->IsActive()) continue;

                bool isRectC = std::holds_alternative<RectangleShape>(body.collider->GetShape());
                float rotC = body.collider->GetOwner()->transform.GetRotation2D() * DEG2RAD;

                Vector2f globalPosC = body.collider->GetOwner()->transform.GetGlobalPosition();
                Vector2f posC = globalPosC + (Matrix3x3::Rotation(rotC) * body.collider->GetOffset());

                Physics::CollisionManifold manifold;

                if (isRectT && isRectC)        manifold = Physics::CheckCollision(obbT, obbCache[i]);
                else if (!isRectT && !isRectC) manifold = Physics::CheckCollision(posT, std::get<CircleShape>(trigger->GetShape()).radius, posC, std::get<CircleShape>(body.collider->GetShape()).radius);
                else if (!isRectT && isRectC)  manifold = Physics::CheckCollision(posT, std::get<CircleShape>(trigger->GetShape()).radius, obbCache[i]);
                else
                {
                    manifold = Physics::CheckCollision(posC, std::get<CircleShape>(body.collider->GetShape()).radius, obbT);
                    if (manifold.isColliding) manifold.normal = -manifold.normal;
                }

                if (manifold.isColliding)
                {
                    trigger->SetDebugColor({ 255, 165, 0, 255 });
                    trigger->EmitTriggerEnter(body.collider->GetOwner());
                }
            }
        }
    }
}