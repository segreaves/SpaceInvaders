# Space Invaders
Welcome to Space Invaders, a classic arcade-style game in the visual-style of old Apple II computers developed entirely in C++ with the SFML library. This project was an opportunity to learn about low-level game development with C++17 while implementing fundamental programming concepts and design patterns.

![image](https://github.com/user-attachments/assets/55e64e6a-205a-42b3-b3e7-a6d5cabdb3c3)

## Features
![image](https://github.com/user-attachments/assets/ea3d5e55-9df1-4e8e-a3d6-8dafbf797e41)
Classic Gameplay: Waves of alien invaders to battle with a responsive player ship.
Efficient Rendering: Smooth 2D graphics using SFML.
Architectural Design: Modular and scalable codebase leveraging industry-standard patterns.

## How to play
![image](https://github.com/user-attachments/assets/72d08b47-153a-4e43-b797-26d79b38843c)
Go to the "SpaceInvaders - Playable" folder. The contents of this folder can be downloaded and are all that is required to play the game. Make sure the .dll files and the "assets" folder are in the same location as the SpaceInvaders.exe file.
### Controls
Use the mouse to move the ship and left-mouse button to fire. The 'H' key provides details on options available.

## Acknowledgements
### Music:
- Intro music: introwavybgm by Monster Logix Studio (https://opengameart.org/content/retro-space-bgm)
- Game music: Grey_Sector_v0_86 by FoxSynergy (https://opengameart.org/content/retro-space-bgm)
### Sounds:
- "SFX- The Ultimate 2017 16 bit Mini pack" by phoenix1291 (https://opengameart.org/content/sfx-the-ultimate-2017-16-bit-mini-pack)
- "Misc Lasers: by Muncheybobo (https://opengameart.org/content/retro-shooter-sound-effects)
- "space_ships" by setaiso (https://opengameart.org/content/space-ships-sounds-and-bg-music)

![image](https://github.com/user-attachments/assets/a5d202a3-d8f0-49a0-a41a-f393b90d1f65)
Thank you to Pezzza's Work (https://www.youtube.com/@PezzzasWork) for sharing invaluable advice and for inspiring this project through his own 'Invaders From Space' project. This project draws heavily from his approach in this project.

![image](https://github.com/user-attachments/assets/819186db-8b41-4b13-adb6-653af70133c9)
A lot of the knowledge implemented in this project comes from the 'SFML Game Development By Example' book by Raimondas Pupius, highly recommended to understand the huge advantages of things like the Entity-Component-System pattern, and others.

## Architecture
![image](https://github.com/user-attachments/assets/33db61b2-b909-4701-816a-c5b26bd38b9a)
This project employs several modern software design patterns to ensure maintainability and scalability:

### Entity-Component-System (ECS)
The ECS pattern is implemented throughout the project to manage game entities, their components, and the systems that operate on them. It decouples game objects from their behaviors and data.

### State Machine
Implemented through the State, StateManager, and various derived state classes. Organizes game states (e.g., intro, game, pause, game-over) into manageable units, each with its own logic.

States transition based on specific conditions or events.
Only one state is active at a time, simplifying updates and rendering.
Though only one state is active, this project implements a 'layered' state machine, where states can be 'transparent'. This allows for the rendering of multiple states at the same time. This is useful when intending to render the background state (such as paused gameplay) while viewing the current state in the foreground (such as the paused text).

### Observer
This pattern allows different parts of the system to communicate with each other by subscribing to and receiving notifications about specific events.

Key Components:
- MessageHandler: Manages the subscription and dispatching of messages to subscribers. It allows systems to subscribe to specific 'channels' and receive notifications when those channels dispatch messages.
- ISubscriber Interface: An interface that defines the notify method, which is called when a message is dispatched to the subscriber.
- Systems as Subscribers: Various Sys classes (e.g., Sys_Collision, Sys_InvaderControl, Sys_Health) implement the ISubscriber interface and subscribe to specific message types to receive notifications and handle events.

### Singleton
A pattern that ensures a class has only one instance and provides global access to it.

Used for resource management (e.g., textures, sounds) in this project.
Benefit: Simplifies access to shared resources while avoiding redundant loading or initialization (e.g., rendering multiple invaders/bullets that share the same texture).

### Factory
Encapsulates the logic for creating objects, with varying configurations.

Used to instantiate game entities like enemies and bullets, as well as game states. Allowing for centralized object creation logic, simplifying modifications and extensions.

### Command
The Controller class uses the Command pattern to map user input events to specific actions or commands. Each input event (e.g., key press, mouse movement) triggers a corresponding Trigger object, which then executes the associated callback function.

![image](https://github.com/user-attachments/assets/6f229755-bdb8-457a-b57a-42687be96a47)
