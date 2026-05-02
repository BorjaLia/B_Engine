#include "PhysicsSystem.h"

#include <algorithm>

#include "ICollisionListener.h"
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
            ColliderComponent* colA;
            ColliderComponent* colB;
            Physics::CollisionManifold manifold;
        };

        void ResolveVelocity(CollisionPair& pair)
        {
            auto* bodyA = pair.colA->GetOwner()->GetComponent<RigidBodyComponent>();
            auto* bodyB = pair.colB->GetOwner()->GetComponent<RigidBodyComponent>();

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

            Vector2f centerOfMassA = pair.colA->GetOwner()->GetGlobalPosition();
            Vector2f centerOfMassB = pair.colB->GetOwner()->GetGlobalPosition();

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
            auto* bodyA = pair.colA->GetOwner()->GetComponent<RigidBodyComponent>();
            auto* bodyB = pair.colB->GetOwner()->GetComponent<RigidBodyComponent>();

            // Only Dynamic bodies get nudged — Static and Kinematic stay put
            float invMassA = (bodyA && bodyA->GetType() == BodyType::Dynamic) ? 1.0f / bodyA->GetMass() : 0.0f;
            float invMassB = (bodyB && bodyB->GetType() == BodyType::Dynamic) ? 1.0f / bodyB->GetMass() : 0.0f;

            if (invMassA == 0.0f && invMassB == 0.0f) return;

            // percent: fraction of penetration corrected per step. 0.35 avoids jitter at rest.
            // slop: allowed penetration before correction fires. Prevents float noise jitter.
            const float percent = 0.35f;
            const float slop = 0.05f;

            Vector2f correction = pair.manifold.normal *
                (std::max(pair.manifold.depth - slop, 0.0f) / (invMassA + invMassB)) * percent;

            if (invMassA > 0.0f)
                pair.colA->GetOwner()->transform->SetPosition(
                    pair.colA->GetOwner()->transform->GetPosition() + (correction * invMassA));

            if (invMassB > 0.0f)
                pair.colB->GetOwner()->transform->SetPosition(
                    pair.colB->GetOwner()->transform->GetPosition() - (correction * invMassB));
        }

        void DispatchCollisionEvents(CollisionPair& pair)
        {
            for (const auto& comp : pair.colA->GetOwner()->GetAllComponents())
            {
                if (auto* listener = dynamic_cast<ICollisionListener*>(comp.get()))
                {
                    listener->OnCollision(pair.manifold);
                }
            }

            Physics::CollisionManifold invertedHit = pair.manifold;
            invertedHit.normal = -pair.manifold.normal;

            for (const auto& comp : pair.colB->GetOwner()->GetAllComponents())
            {
                if (auto* listener = dynamic_cast<ICollisionListener*>(comp.get()))
                {
                    listener->OnCollision(invertedHit);
                }
            }
        }
    } // End Anonymous Namespace

    PhysicsSystem::PhysicsSystem()
    {
        activeColliders.reserve(1000);
        activeTriggers.reserve(100);
        obbCache.reserve(1000);
    }

    void PhysicsSystem::Update(Node* rootScene, float fixedDeltaTime)
    {
        (void)fixedDeltaTime;
        if (!rootScene) return;

        PrePass(rootScene);
        SolveCollisions();
        DetectTriggers();
    }

    void PhysicsSystem::PrePass(Node* rootScene)
    {
        activeColliders.clear();
        activeTriggers.clear();
        obbCache.clear();

        rootScene->GetAllColliders(activeColliders);
        rootScene->GetAllTriggerAreas(activeTriggers);

        for (auto* col : activeColliders)
        {
            col->SetDebugColor({ 0, 255, 0, 255 });

            if (std::holds_alternative<RectangleShape>(col->GetShape()))
            {
                const auto rect = std::get<RectangleShape>(col->GetShape());
                float rot = col->GetOwner()->transform->GetRotation() * (3.14159f / 180.0f);
                Vector2f rotatedOffset = Matrix3x3::Rotation(rot) * col->GetOffset();
                Vector2f pos = col->GetOwner()->GetGlobalPosition() + rotatedOffset;

                obbCache.push_back(Physics::GetOBB(pos, rect.size, rot));
            }
            else
            {
                obbCache.push_back(Physics::OBB{});
            }
        }
    }

    void PhysicsSystem::SolveCollisions()
    {
        std::vector<CollisionPair> contacts;
        contacts.reserve(100);

        size_t count = activeColliders.size();
        for (size_t i = 0; i < count; ++i)
        {
            for (size_t j = i + 1; j < count; ++j)
            {
                auto* colA = activeColliders[i];
                auto* colB = activeColliders[j];

                Shape shapeA = colA->GetShape();
                Shape shapeB = colB->GetShape();

                bool isRectA = std::holds_alternative<RectangleShape>(shapeA);
                bool isRectB = std::holds_alternative<RectangleShape>(shapeB);

                float rotA = colA->GetOwner()->transform->GetRotation() * (3.14159f / 180.0f);
                float rotB = colB->GetOwner()->transform->GetRotation() * (3.14159f / 180.0f);

                Vector2f posA = colA->GetOwner()->GetGlobalPosition() + (Matrix3x3::Rotation(rotA) * colA->GetOffset());
                Vector2f posB = colB->GetOwner()->GetGlobalPosition() + (Matrix3x3::Rotation(rotB) * colB->GetOffset());

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
                    contacts.push_back({ colA, colB, manifold });
                    colA->SetDebugColor({ 255, 0, 0, 255 });
                    colB->SetDebugColor({ 255, 0, 0, 255 });
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
            trigger->SetDebugColor({ 255, 255, 0, 150 });

            bool isRectT = std::holds_alternative<RectangleShape>(trigger->GetShape());
            float rotT = trigger->GetOwner()->transform->GetRotation() * (3.14159f / 180.0f);
            Vector2f posT = trigger->GetOwner()->GetGlobalPosition() + (Matrix3x3::Rotation(rotT) * trigger->GetOffset());

            Physics::OBB obbT;
            if (isRectT) obbT = Physics::GetOBB(posT, std::get<RectangleShape>(trigger->GetShape()).size, rotT);

            for (size_t i = 0; i < activeColliders.size(); ++i)
            {
                auto* col = activeColliders[i];
                bool isRectC = std::holds_alternative<RectangleShape>(col->GetShape());
                float rotC = col->GetOwner()->transform->GetRotation() * (3.14159f / 180.0f);
                Vector2f posC = col->GetOwner()->GetGlobalPosition() + (Matrix3x3::Rotation(rotC) * col->GetOffset());

                Physics::CollisionManifold manifold;

                if (isRectT && isRectC)        manifold = Physics::CheckCollision(obbT, obbCache[i]);
                else if (!isRectT && !isRectC) manifold = Physics::CheckCollision(posT, std::get<CircleShape>(trigger->GetShape()).radius, posC, std::get<CircleShape>(col->GetShape()).radius);
                else if (!isRectT && isRectC)  manifold = Physics::CheckCollision(posT, std::get<CircleShape>(trigger->GetShape()).radius, obbCache[i]);
                else
                {
                    manifold = Physics::CheckCollision(posC, std::get<CircleShape>(col->GetShape()).radius, obbT);
                    if (manifold.isColliding) manifold.normal = -manifold.normal;
                }

                if (manifold.isColliding)
                {
                    trigger->SetDebugColor({ 255, 165, 0, 255 });
                    trigger->EmitTriggerEnter(col->GetOwner());
                }
            }
        }
    }
}