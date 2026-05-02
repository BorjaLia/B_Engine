B_Engine and TestGame {#mainpage}

Welcome to the official documentation for B_Engine and its companion test game

This documentation is divided into two main sections to maintain a clear boundary between the engine's architecture and the game's specific logic.

B_Engine

The engine is built around a custom Entity-Component-System (ECS) architecture, divided into the following primary modules:

@ref Core "Core Systems": The heart of the engine (Application, Window, Time, Config, etc.)

@ref Components "ECS Components": All data and behavior modules attachable to a Node (Sprite, Transform, RigidBody, etc.)

@ref Scenes "Scene Management": The Node graph, Scene Builder, and scene transitions.

@ref Events "Event System": The EventBus, CRTP event generation, and system events.

Test Game (Platformer)

A physics-based platformer demonstrating the engine's capabilities.

@ref GameScripts "Game Scripts": Game-specific logic (PlayerScript, EnemyScript, GameManager, etc.)

@ref GamePrefabs "Game Prefabs": Entity builders and assemblers for the game world.

Documentation automatically generated via Doxygen and GitHub Actions.