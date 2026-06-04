/**
 * Game logic and rendering implementation. 
 */
#ifndef GAME_H
#define GAME_H
#include "candy.h"
#include "graphics.h"
#include "controller.h"
#include "board.h"
#include <random>
#include <vector>
#include "sound.h"
#include "sound_mgr.h"

const int DEFAULT_BLOCKSIZE = 3;
const int NUM_CANDYTYPES = 6;

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

    /**
     * Appends to the game score given the exploded candies.
     */
    void scoreUpdate(const std::vector<Candy*>& candies);

    /// @return true if this game is equal to the other game (same board state and falling block)
    bool operator==(const Game& other) const;

    /**
     * Returns the score.
     */
    int getScore() const { return m_score; }

    /**
     * Runs game-over logic; sets internal var to true.
     */
    void gameOver();

private:
    Board m_board;
    Candy** m_blockCandy;
    std::mt19937 m_gen;

    int m_x;
    int m_y;
    int m_speedCounter;
    bool m_landed;
    bool m_gameOver;
    bool m_pause;
    int m_score;
    SoundManager m_sound;
    SoundManager::MusicToken m_sound_stars;
};
#endif
