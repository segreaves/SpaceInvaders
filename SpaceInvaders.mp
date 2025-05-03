# Technical Documentation for SpaceInvaders Project

## 1. Architecture Overview
- **Entity-Component-System (ECS) Pattern**:
  - Implements ECS to manage game entities and their behaviors through components and systems.
  
- **State Machine for Game Flow**:
  - Utilizes a state machine to handle different game states such as Intro, Game, Paused, and GameOver.
  
- **Separation of Concerns**:
  - Separates game logic into distinct systems, enhancing modularity and maintainability.

## 2. Core Systems
- **Engine Class**:
  - **Function**: Manages the main game loop and overall game context.
  - **Key Features**:
    - Initializes the rendering window, state manager, system manager, texture manager, audio manager, and sound manager.
    - Handles game loop phases: `update()`, `render()`, and `lateUpdate()`.
    - Manages post-processing effects like bloom and render lines.
    - Maintains rendering textures and shaders for visual enhancements.

- **StateManager Class**:
  - **Function**: Handles transitions between different game states.
  - **Key Features**:
    - Registers various states like Intro, Game, Paused, and GameOver.
    - Maintains a stack of active states.
    - Manages activation and deactivation of states.
    - Ensures proper cleanup and transition logic during state changes.

- **SystemManager Class**:
  - **Function**: Manages different game systems responsible for specific game mechanics.
  - **Key Systems**:
    - **Movement System (`Sys_Movement`)**: Handles entity movements based on position and velocity.
    - **Collision System (`Sys_Collision`)**: Detects and manages collisions between entities.
    - **Rendering System (`Sys_Renderer`)**: Manages the rendering pipeline and draws entities.
    - **Sound System (`Sys_Sound`)**: Manages audio playback and sound effects.
    - **Invader Control System (`Sys_InvaderControl`)**: Manages invader behaviors and spawning.
    - **Player Control System (`Sys_PlayerControl`)**: Handles player input and movement.
    - **Bullet Control System (`Sys_BulletControl`)**: Manages bullet creation and movement.
    - **Health System (`Sys_Health`)**: Tracks entity health and handles destruction.
    - **Gravity System (`Sys_Gravity`)**: Applies gravitational forces to entities.
    - **Spring System (`Sys_Spring`)**: Implements spring physics for certain entities.
    - **Ship Sway System (`Sys_ShipSway`)**: Creates realistic ship movement effects.
    - **UFO Control System (`Sys_UFOControl`)**: Manages UFO behavior and spawning.
    - **Bunker Control System (`Sys_BunkerControl`)**: Handles bunker damage and destruction.
    - **Shockwave Control System (`Sys_ShockwaveControl`)**: Manages shockwave effects.
    - **Level State System (`Sys_LevelState`)**: Tracks level progression and game state.

## 3. Game Loop
The game's main loop is implemented in the `Engine::run()` method and follows this structure:

```cpp
void Engine::run()
{
    while (m_windowManager.isOpen())
    {
        update();
        render();
        lateUpdate();
    }
}
```

- **Phases**:
  - **Update**:
    - Handles game state updates, entity behaviors, and processes input.
    - Calls `StateManager::update()` to update the current game state.
    
  - **Render**:
    - Clears the window.
    - Draws the current state.
    - Applies post-processing effects like bloom.
    
  - **Late Update**:
    - Synchronizes frame timing.
    - Restarts the clock for the next frame.

### 3.1 Engine Class Details
- **Member Variables**:
  - **Context (`m_context`)**: Shared context for communication between components and systems.
  - **Managers**:
    - `WindowManager`: Handles window creation and rendering context.
    - `StateManager`: Manages game states and transitions.
    - `SysManager`: Oversees various game systems.
    - `TextureManager`, `AudioManager`, `SoundManager`: Manage textures, audio assets, and sound playback respectively.
    - `ActorManager`: Manages game entities and their components.
  - **Rendering Textures and Shaders**:
    - `m_windowTexture`, `m_brightnessTexture`, `m_blurTexture1`, `m_blurTexture2`: Used for post-processing effects.
    - `m_brightnessShader`, `m_blurShader`: Shaders for bloom effect implementation.

- **Key Methods**:
  - **update()**: Updates game logic and state.
  - **render()**: Handles rendering of game entities and post-processing.
  - **lateUpdate()**: Manages frame synchronization and timing.
  - **afterEffects()**: Applies post-processing effects like bloom and render lines.
  - **addBloom()**: Implements the bloom effect using shaders and render textures.
  - **addRenderLines()**: Draws visual debug lines to assist in alignment and debugging.

### 3.2 SystemManager Implementation
- **Fixed Timestep Physics**:
  - Uses an accumulator-based approach for fixed timestep physics updates.
  - Physics systems update at 120Hz (1/120 seconds per step) for consistent simulation.
  
- **System Execution Order**:
  - Systems are executed in a specific order to ensure proper dependencies:
    1. Gravity System
    2. Spring System
    3. Movement System
    4. Player Control System
    5. Bullet Control System
    6. Bunker Control System
    7. Shockwave Control System
    8. UFO Control System
    9. Invader Control System
    10. Ship Sway System
    11. Torque Spring System
    12. Rotation System
    13. Collision System
    14. Sound System
    15. Health System
    16. Level State System
  
- **Event Handling**:
  - Implements a queue-based event system for actor events.
  - Events are processed during the fixed update step.
  - Each system can respond to events relevant to its functionality.

## 4. Rendering System
- **Double-Buffered Rendering**:
  - Utilizes two render textures to manage post-processing effects, ensuring smooth visual transitions.
  
- **Bloom Effect Implementation**:
  - Extracts bright areas from the scene.
  - Applies horizontal and vertical blur passes using shaders.
  - Combines the blurred bright areas with the original scene for the bloom effect.
  
- **Debug Visualization**:
  - Draws render lines for debugging purposes to visualize game elements and states.

## 5. Key Classes
- **Actor Class (`Actor.cpp`, `Actor.h`)**:
  - **Function**: Base class for all game entities.
  - **Components**: Position (`Comp_Position`), Movement (`Comp_Movement`), Invader (`Comp_Invader`), etc.
  
- **Component Class (`Comp_*.cpp`, `Comp_*.h`)**:
  - **Function**: Data containers that define properties of entities.
  - **Examples**: 
    - `Comp_Position`: Manages entity positions.
    - `Comp_Movement`: Handles movement logic and velocity.
    - `Comp_Invader`: Specific properties for invader entities.
  
- **State Classes (`State_*.cpp`, `State_*.h`)**:
  - **Function**: Represent different states of the game (e.g., Intro, Game, Paused, GameOver).
  - **Examples**:
    - `State_Game`: Manages the main gameplay.
    - `State_Intro`: Handles the introductory screen.
    - `State_Paused`: Manages the paused state.
    - `State_GameOver`: Handles the game over screen.
  
- **System Classes (`Sys_*.cpp`, `Sys_*.h`)**:
  - **Function**: Logic handlers for specific game mechanics.
  - **Examples**:
    - `Sys_Movement`: Updates entity positions based on movement components.
    - `Sys_Collision`: Detects and handles collisions between entities.
    - `Sys_Renderer`: Manages rendering of entities and application of visual effects.
    - `Sys_InvaderControl`: Controls invader behaviors and spawning logic.
    - `Sys_Sound`: Manages sound effects and music playback.

### 5.1 State_Game Class Details
- **Function**: Manages the main gameplay state.
- **Key Features**:
  - **Initialization (`onCreate()`)**:
    - Sets up the game view and HUD.
    - Loads and plays game music.
    - Initializes player icons and positions.
  
  - **Update Logic (`update()`)**:
    - Checks invader count and loads next level if all invaders are defeated.
    - Processes player input and updates game systems.
    - Updates HUD elements like score, level, and FPS.
    - Handles game over conditions based on player lives and invader invasion.
  
  - **Rendering (`draw()`)**:
    - Draws the game background and entities.
    - Renders HUD elements and player indicators.
    - Optionally draws FPS and help panels for debugging.
  
  - **Activation and Deactivation**:
    - **activate()**: Sets up input callbacks for player actions like shooting, pausing, and toggling sound/music. Initiates a new game if necessary.
    - **deactivate()**: Removes input callbacks to clean up when the state is no longer active.
  
  - **Level Management**:
    - **loadNextLevel()**: Increments the level, resets invader count, and spawns new invaders.
    - **newGame()**: Resets game state, purges existing actors, initializes level data, and sets up player entities.
  
  - **HUD and UI Management**:
    - **initializeHUD()**: Sets up HUD text elements for score, level, kills, FPS, and help indicators.
    - **updateHUD()**: Updates HUD elements based on game state and player actions.
  
  - **Input Handling**:
    - **handlePlayerPosition()**: Manages player movement based on mouse input.
    - **onPlayerShoot()**: Handles shooting actions triggered by player input.
    - **onToggleHelp(), onToggleSound(), onToggleMusic()**: Toggles help panels, sound effects, and music respectively.

## 6. User Interface and HUD
- **HUD Elements**:
  - Displays score, level, kills, and FPS.
  - Shows player lives and help panels.
  
- **Input Handling**:
  - Processes player inputs for movement, shooting, pausing, toggling sound/music, etc.
  - Handled through the `Controller` class which manages input callbacks.

## 7. Sound and Music
- **SoundManager Class (`SoundManager.cpp`, `SoundManager.h`)**:
  - **Function**: Manages loading and playback of sound effects and music.
  - **Features**:
    - Load sound profiles.
    - Play and switch music tracks.
    - Toggle sound effects and music on/off.

## 8. Level Management
- **LevelManager Class (`LevelManager.cpp`, `LevelManager.h`)**:
  - **Function**: Handles level progression, spawning of invaders, and managing player lives.
  - **Features**:
    - Tracks current level, score, kills, and player lives.
    - Spawns invaders based on level data.
    - Manages transitions between levels and game over states.

## 9. Shaders and Effects
- **Shaders**:
  - `brightness.frag` and `blur.frag` implemented for post-processing effects.
  
- **Effects**:
  - **Bloom**: Enhances visual quality by emphasizing bright elements through a multi-pass blur process.
  - **Render Lines**: Assists in debugging spatial elements and alignment by drawing horizontal lines.

## 10. Level Progression and Game Over Logic
- **Level Progression**:
  - Upon clearing all invaders, the game loads the next level with increased difficulty.
  - Invaders are queued and spawned using the `Sys_InvaderControl` system.
  
- **Game Over Conditions**:
  - Triggered when the player's lives reach zero or invaders invade.
  - Transitions to the `GameOver` state, displays the game over screen, and prepares for a new game.

## 11. Post-Processing and Visual Enhancements
- **Bloom Effect**:
  - Extracts bright areas and applies horizontal and vertical blurs.
  - Combines blurred textures with the original scene to create a glowing effect.
  
- **Render Lines**:
  - Draws semi-transparent lines across the screen for visual debugging.
  - Helps in aligning game entities and visual elements.

## 12. Input and Control Management
- **Controller Class (`Controller.cpp`, `Controller.h`)**:
  - **Function**: Manages player inputs and maps them to game actions.
  - **Features**:
    - Handles input callbacks for actions like shooting, pausing, toggling help, sound, and music.
    - Interfaces with the `StateManager` to trigger state transitions based on input.

## 13. Context and Managers
- **Context Class (`Context.h`)**:
  - **Function**: Provides a shared context for communication between different managers and systems.
  
- **Managers**:
  - **WindowManager**: Manages the game window and rendering context.
  - **StateManager**: Oversees game state transitions and active state stack.
  - **SystemManager**: Coordinates various game systems for updating and rendering.
  - **TextureManager**: Handles loading and management of game textures.
  - **AudioManager**: Manages audio resources and playback.
  - **SoundManager**: Specifically handles sound effects and music.
  - **ActorManager**: Responsible for creating, managing, and updating game entities and their components.

## 14. Actor and Component Management
- **Actor Class**:
  - Represents all game entities, each composed of multiple components.
  
- **Component Classes (`Comp_*.cpp`, `Comp_*.h`)**:
  - Define specific properties and data for actors, such as position, movement, and invader-specific attributes.

## 15. System Operations
- **Movement System (`Sys_Movement`)**:
  - Updates entity positions based on their movement components.

- **Collision System (`Sys_Collision`)**:
  - Detects and processes collisions between entities, handling game logic like health reduction or entity destruction.

- **Renderer System (`Sys_Renderer`)**:
  - Manages rendering of all visible entities, applying visual effects and ensuring correct draw order.

- **Invader Control System (`Sys_InvaderControl`)**:
  - Manages behaviors specific to invaders, including movement patterns and spawning new invaders.

- **Sound System (`Sys_Sound`)**:
  - Controls playback of sound effects and background music, ensuring audio synchronization with game events.

## 16. Physics and Special Effects
- **Gravity System (`Sys_Gravity`)**:
  - Applies gravitational forces to entities with mass components.
  
- **Spring System (`Sys_Spring`)**:
  - Implements spring physics for entities, creating realistic oscillation effects.
  
- **Torque Spring System (`Sys_TorqueSpring`)**:
  - Applies rotational spring forces to create realistic rotational movement.
  
- **Ship Sway System (`Sys_ShipSway`)**:
  - Creates realistic swaying motion for the player's ship, enhancing visual feedback.
  
- **Shockwave System (`Sys_ShockwaveControl`)**:
  - Manages visual and gameplay effects of explosions and shockwaves.

## Conclusion
The SpaceInvaders project is a robust C++ implementation of the classic Space Invaders game, leveraging modern design patterns like ECS and state machines. With modular systems managing different aspects of the game, it ensures a maintainable and scalable codebase. The rendering pipeline utilizes advanced techniques to provide a visually appealing experience, while the sound management system enhances the overall gameplay. The game loop efficiently handles updates, rendering, and synchronization, ensuring smooth and responsive gameplay.

# End of Documentation
