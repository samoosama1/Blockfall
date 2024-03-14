#include <fstream>
#include <iostream>

#include "GameController.h"

void exitRoutine(BlockFall &game);
bool GameController::play(BlockFall& game, const string& commands_file){
    // TODO: implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. the return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.

    std::ifstream ifs{commands_file};
    std::string command;

    if(!ifs.is_open()) {
        std::cerr << "Error opening file: " << commands_file << "\n";
        exit(-1);
    }

    game.place_block(game.initial_block);

    while(ifs >> command ) {
        if(game.game_over)
            break;
        if(command == "PRINT_GRID") {
            game.print_grid();
            std::cout << "\n";
            continue;
        }
        if(command == "ROTATE_RIGHT") {
            game.rotate_right();
            continue;
        }
        if(command == "ROTATE_LEFT") {
            game.rotate_left();
            continue;
        }
        if(command == "MOVE_RIGHT") {
            game.move_right();
            continue;
        }
        if(command == "MOVE_LEFT") {
            game.move_left();
            continue;
        }
        if(command == "DROP") {
            game.drop();
            switch (game.deploy_block()) {
                case 'w':
                    std::cout << "You won! Out of blocks." << "\n";
                    exitRoutine(game);
                    return true;
                case 'l':
                    std::cout << "You lose! Block couldn't be deployed." << "\n";
                    exitRoutine(game);
                    return false;
            }
            continue;
        }
        if(command == "GRAVITY_SWITCH") {
            game.switch_gravity();
            continue;
        }
        std::cout << "Unknown command: " << command;
    }
    game.remove_block();
    exitRoutine(game);
    std::cout << "Out of commands!" << "\n";
    return true;
}

void exitRoutine(BlockFall &game) {
    game.leaderboard.insert_new_entry(new LeaderboardEntry{game.current_score, time(nullptr), game.player_name});
    game.leaderboard.write_to_file(game.leaderboard_file_name);
}
