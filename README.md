# ChessForge (Chess for Linux)

This project is a chess game developed using the raylib library for the Linux operating system. I am aiming to add a chess engine too in the near future to enhance the gameplay experience.

## Project Structure
The project is organized into the following directories:

- **src/**: Contains the source code for the chess game.
  - **main.cpp**: Entry point of the application.
  - **game/**: Contains classes related to the chess game logic, including the board and pieces.
  - **engine/**: Contains the chess engine logic, including move generation and evaluation.
  - **ui/**: Contains classes for rendering the game and user interface.

- **assets/**: Contains additional resources needed for the game, such as images or sound files.

- **CMakeLists.txt**: Configuration file for building the project with CMake.

## Setup Instructions (Linux only)
1. Ensure you have CMake, raylib and stockfish installed on your system.
   - For Arch based Distros (in case the package is not in pacman use yay or paru): 
      ```
      sudo pacman -S raylib cmake stockfish
      ```

2. Clone the repository or download the project files.
3. Navigate to the project directory and execute this command (giving permission to the executable) :
   ```
   chmod +x run.sh
   ```
4. For running the Game :
   ```
   ./run.sh
   ```
5. For cleaning the build artifacts :
   ```
   ./run.sh clean
   ```
## Gameplay
The game allows two players to play chess against each other. The chessboard is displayed using raylib, and players can move pieces by clicking on them and selecting their destination squares. 

## Future Development Plans
- Implement a chess engine to allow for computer gameplay.
- [Done] Adding computer bot that uses openSource engine (Stockfish, Halogen) for its moves.
- Add support for different chess variants.
- Enhance the user interface with additional features such as move history and player statistics.
- Optimize the engine for better performance and accuracy in move generation and evaluation.