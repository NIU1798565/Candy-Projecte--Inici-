/**
 * Game logic and rendering implementation. 
 */
#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "controller.h"
#include "board.h"
#include "candy.h"
#include <string>

/**
 * Main game class: keep track of the game state it. 
 * When run_graphic_game() is called, the game loop will call
 * update() and render() every frame, in that order.
 */
class Game
{
public:
    Game();

    ~Game();

    /// Run the game loop
    void run();

    /**
     * Update the game state. Called every frame when run().
     * This part is not expected to do any rendering.
     * 
     * @param controller the Controller to use for input handling.
     */
    void update(const Controller& controller);

    /**
     * Draw the next frame. Called once per frame, after update().
     * 
     * @param graphics the GraphicManager to use for rendering.
     */
    void render(GraphicManager& graphics);

    /**
     * Serialize and write the current game state (board and falling block) to a file.
     * @param output_path path where to save the game state.
     * @return true if the dump was successful, false otherwise.
     */
    bool dump(const std::string& output_path) const;

    /**
     * Load a serialized game state (board and falling block) from a file created with dump().
     * @param input_path path from which to load the game state.
     * @return true if the load was successful, false otherwise.
     */
    bool load(const std::string& input_path);

    /// @return true if this game is equal to the other game (same board state and falling block)
    bool operator==(const Game& other) const;
private:
    Board m_board;

    // Bloc que cau 2 caramels
    // m_falling[0] = peça superior (si vertical) o esquerra (si horitzontal)
    // m_falling[1] = peça inferior o dreta
    Candy* m_falling[2];
    int m_fallingX;   // columna del tauler de m_falling[0]
    int m_fallingY;   // fila del tauler de m_falling[0]
    bool m_vertical;  // true = bloc vertical, false = horitzontal

    int m_puntuacio;
    int m_fallTimer;  // frames fins al proper descens automàtic
    bool m_gameOver;
};
#endif
