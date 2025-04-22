cmake_minimum_required(VERSION 3.15)
project(SpaceInvaders)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
set(SOURCES
  Actor.cpp
  ActorManager.cpp
  Controller.cpp
  Engine.cpp
  Grid.cpp
  LevelManager.cpp
  Particle.cpp
  ParticleSystem.cpp
  SoundManager.cpp
  SpaceInvaders.cpp
  SpriteSheet.cpp
  StateManager.cpp
  State_Game.cpp
  State_GameOver.cpp
  State_Intro.cpp
  State_Paused.cpp
  Sys.cpp
  SysManager.cpp
  Sys_BunkerControl.cpp
  Sys_Gravity.cpp
  Sys_Health.cpp
  Sys_LevelState.cpp
  Sys_Rotation.cpp
  Sys_ShockwaveControl.cpp
  Sys_InvaderControl.cpp
  Sys_Collision.cpp
  Sys_BulletControl.cpp
  Sys_PlayerControl.cpp
  Sys_Movement.cpp
  Sys_Renderer.cpp
  Sys_Sound.cpp
  Sys_Spring.cpp
  Sys_ShipSway.cpp
  Sys_TorqueSpring.cpp
  Sys_UFOControl.cpp
  WindowManager.cpp
)

set(HEADERS
  Actor.h
  ActorEventType.h
  ActorManager.h
  ActorMessageType.h
  AudioManager.h
  Channel.h
  Comp_Bullet.h
  Comp_Bunker.h
  Comp_Collision.h
  Comp_Control.h
  Comp_Grid.h
  Comp_Health.h
  Comp_Invader.h
  Comp_Mass.h
  Comp_Movement.h
  Comp_Particles.h
  Comp_Player.h
  Comp_Position.h
  Comp_ShipSway.h
  Comp_Shockwave.h
  Comp_SoundEmitter.h
  Comp_Spring.h
  Comp_SpriteSheet.h
  Comp_Target.h
  Comp_TorqueSpring.h
  Comp_UFO.h
  Comp.h
  ComponentType.h
  Context.h
  Controller.h
  Direction.h
  Engine.h
  Grid.h
  IDrawable.h
  ISubscriber.h
  LevelManager.h
  Message.h
  MessageHandler.h
  OriginType.h
  Params.h
  Particle.h
  ParticleSystem.h
  ShapeType.h
  SoundManager.h
  SoundType.h
  SpriteSheet.h
  State.h
  State_Game.h
  State_GameOver.h
  State_Intro.h
  State_Paused.h
  StateType.h
  Sys.h
  SysManager.h
  Sys_BunkerControl.h
  Sys_Gravity.h
  Sys_Health.h
  Sys_LevelState.h
  Sys_Rotation.h
  Sys_ShockwaveControl.h
  Sys_InvaderControl.h
  Sys_Collision.h
  Sys_BulletControl.h
  Sys_PlayerControl.h
  Sys_Movement.h
  Sys_Renderer.h
  Sys_Sound.h
  Sys_Spring.h
  Sys_ShipSway.h
  Sys_TorqueSpring.h
  Sys_UFOControl.h
  SystemType.h
  TextureManager.h
  Trigger.h
  Utils.h
  WindowManager.h
)

# Detect platform and set platform-specific flags
if(APPLE)
  add_definitions(-DRUNNING_MACOS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  set(SFML_ROOT "/usr/local/opt/sfml") # Adjust if SFML is installed elsewhere
  # Removed find_package for SFML due to version 3.0 CMake issues
  include_directories(/opt/homebrew/include)
  link_directories(/opt/homebrew/lib)
elseif(WIN32)
  add_definitions(-DRUNNING_WINDOWS)
endif()

add_executable(SpaceInvaders ${SOURCES} ${HEADERS})

if(APPLE)
  target_link_libraries(SpaceInvaders sfml-graphics sfml-window sfml-system sfml-audio sfml-network)
endif()

# Additional macOS linking for frameworks if needed
if(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(COREVIDEO_LIBRARY CoreVideo)
  target_link_libraries(SpaceInvaders ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()
