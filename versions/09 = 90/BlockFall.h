#ifndef BLOCKFALL_H
#define BLOCKFALL_H

#define occupiedCellChar "⬜"
#define unoccupiedCellChar "⬛"

#include <vector>
#include <string>
#include <iostream>

#include "Block.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"

using namespace std;

class BlockFall {
public:
    BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name,
              const string &player_name);
    virtual ~BlockFall();

    int rows;  // Number of rows in the grid
    int cols;  // Number of columns in the grid
    int currCol;
    vector<vector<int>> grid;  // 2D game grid
    vector<vector<bool>> power_up; // 2D matrix of the power-up shape
    Block * initial_block = nullptr; // Head of the list of game blocks. Must be filled up and initialized after a call to read_blocks()
    Block * active_rotation = nullptr; // Currently active rotation of the active block. Must start with the initial_block
    bool gravity_mode_on = false; // Gravity mode of the game
    bool game_over = false;
    unsigned long current_score = 0; // Current score of the game
    string leaderboard_file_name; // Leaderboard file name, taken from the command-line argument 5 in main
    string player_name; // Player name, taken from the command-line argument 6 in main
    Leaderboard leaderboard;

    void initialize_grid(const string & input_file); // Initializes the grid using the command-line argument 1 in main
    void read_blocks(const string & input_file); // Reads the input file and calls the read_block() function for each block;
    char deploy_block();
    void drop();
    void rotate_right();
    void rotate_left();
    void move_right();
    void move_left();
    bool can_be_placed(Block* block);
    void place_block(Block *block, int row = 0);
    void remove_block();
    bool scan_for_power_up();
    bool check_power_up(int row, int col);
    void clean();
    void scan_rows(bool drop);
    void prompt_clearing();
    void scan_cols();
    bool is_row_full(int row);
    bool is_row_empty(int row);
    void shift_rows(int emptyRow);
    void realign_grid();
    void update_grid(bool drop);
    void switch_gravity();
    int find_row();
    void add_drop_points(int rowToDrop);
    void print_grid();
};



#endif // BLOCKFALL_H
