#include "BlockFall.h"
#include <fstream>
#include <sstream>
#include <iostream>

Block* rotate90CW(Block* block);
bool isNewRotation(Block* head, Block* rotation);
void addRotation(Block* head, Block* rotation);
void createRotations(Block* head);
void deleteList(Block* head);
void deleteInnerList(Block* head);
std::vector<std::vector<std::vector<bool>>> getShapes(std::ifstream &ifs);

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
    std::ifstream ifs{input_file};
    if (!ifs.is_open()) {
        std::cerr << "Error opening file: " << input_file << "\n";
        exit(-1);
    }
    auto shapes = getShapes(ifs);
    for (int i = 0; i < shapes.size() - 1; ++i) {
        Block* newBlock = new Block();
        newBlock->shape = shapes.at(i);
        if(initial_block == nullptr) {
            initial_block = newBlock;
        } else {
            Block* temp = initial_block;
            while(temp->next_block != nullptr) {
                temp = temp->next_block;
            }
            temp->next_block = newBlock;
        }
    }
    power_up = shapes[shapes.size() - 1];
    active_rotation = initial_block;
    for (Block* block = initial_block; block != nullptr; block = block->next_block) {
        createRotations(block);
    }
}

void BlockFall::initialize_grid(const string &input_file) {
    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main
    std::ifstream ifs{input_file};
    std::string line;
    if (!ifs.is_open()) {
        std::cerr << "Error opening file: " << input_file << "\n";
        exit(-1);
    }
    int num;
    while(getline(ifs, line)) {
        std::vector<int> row;
        std::stringstream ss{line};
        while(ss >> num) {
            row.push_back(num);
        }
        grid.push_back(row);
    }
    rows = static_cast<int>(grid.size());
    cols = static_cast<int>(grid[0].size());
    currCol = 0;
}

void BlockFall::print_grid() {
    for (auto &row : grid) {
        for (int i : row) {
            auto str = (i == 1) ? occupiedCellChar : unoccupiedCellChar;
            std::cout << str;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool BlockFall::can_be_placed(Block *block) {
    if(currCol + block->shape[0].size() > cols)
        return false;
    for (int i = 0; i < block->shape.size(); ++i) {
        for (int j = 0; j < block->shape[0].size(); ++j) {
            if(block->shape[i][j] && grid.at(i).at(currCol + j))
                return false;
        }
    }
    return true;
}

void BlockFall::place_block(Block *block, int row) {
    if(!can_be_placed(block))
        return;
    for (int i = 0; i < block->shape.size(); ++i) {
        for (int j = 0; j < block->shape[0].size(); ++j) {
            if(active_rotation->shape.at(i).at(j))
                grid.at(row + i).at(currCol + j) = 1;
        }
    }
}

void BlockFall::remove_block() {
    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[0].size(); ++j) {
            if(active_rotation->shape[i][j])
                grid.at(i).at(currCol + j) = 0;
        }
    }
}

void BlockFall::rotate_right() {
    remove_block();
    if(!can_be_placed(active_rotation->right_rotation)) {
        place_block(active_rotation);
        return;
    }
    active_rotation = active_rotation->right_rotation;
    place_block(active_rotation);
}

void BlockFall::rotate_left() {
    remove_block();
    if(!can_be_placed(active_rotation->left_rotation)) {
        place_block(active_rotation);
        return;
    }
    active_rotation = active_rotation->left_rotation;
    place_block(active_rotation);
}

void BlockFall::move_right() {
    if(currCol + 1 == rows)
        return;
    remove_block();
    currCol++;
    if(!can_be_placed(active_rotation)) {
        currCol--;
    }
    place_block(active_rotation);
}

void BlockFall::move_left() {
    if(currCol - 1 < 0)
        return;
    remove_block();
    currCol--;
    if(!can_be_placed(active_rotation)) {
        currCol++;
    }
    place_block(active_rotation);
}

void BlockFall::drop() {
    remove_block();
    int rowToDrop = find_row();
    add_drop_points(rowToDrop);
    place_block(active_rotation, rowToDrop);
    currCol = 0;
    active_rotation = active_rotation->next_block;
    update_grid(true);
}

int BlockFall::find_row() {
    for (int i = 0; i <= rows - active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape.size(); ++j) {
            for (int k = 0; k < active_rotation->shape[0].size(); ++k) {
                if(active_rotation->shape[j][k] && grid.at(i + j).at(currCol + k)){
                    return i - 1;
                }
            }
        }
    }
    return rows - active_rotation->shape.size();
}

void BlockFall::update_grid(bool drop) {
    if (gravity_mode_on) {
        scan_cols();
    }
    if(scan_for_power_up()) {
        return;
    }
    scan_rows(drop);
    realign_grid();
}

bool BlockFall::scan_for_power_up() {
    for (int i = 0; i <= rows - power_up.size(); ++i) {
        for (int j = 0; j <= cols - power_up[0].size(); ++j) {
            if(check_power_up(i, j)) {
                current_score += 1000;
                clean();
                return true;
            }
        }
    }
    return false;
}

bool BlockFall::check_power_up(int row, int col) {
    for (int k = 0; k < power_up.size(); ++k) {
        for (int l = 0; l < power_up[0].size(); ++l) {
            if(power_up.at(k).at(l) != grid.at(row + k).at(col+l))
                return false;
        }
    }
    return true;
}

void BlockFall::clean() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if(grid[i][j]) {
                current_score += 1;
                grid[i][j] = 0;
            }
        }
    }
}

void BlockFall::scan_rows(bool drop) {
    bool pop = false;
    std::vector<int> fullRows;
    for (int i = 0; i < rows; ++i) {
        if(is_row_full(i)) {
            fullRows.push_back(i);
            pop = true;
        }
    }
    if(drop && pop) {
        prompt_clearing();
    }
    for (int i : fullRows) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = 0;
        }
    }
}

void BlockFall::prompt_clearing() {
    std::cout << "\nBefore clearing:\n";
    print_grid();
}

bool BlockFall::is_row_full(int row) {
    for (int i = 0; i < cols; ++i) {
        if(grid[row][i] == 0)
            return false;
    }
    current_score += cols;
    return true;
}

bool BlockFall::is_row_empty(int row) {
    for (int i = 0; i < cols; ++i) {
        if(grid[row][i] == 1)
            return false;
    }
    return true;
}

void BlockFall::scan_cols() {
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            if(grid.at(j).at(i) == 0) {
                for (int k = j - 1; k >= 0 ; --k) {
                    grid[k + 1][i] = grid[k][i];
                }
                grid[0][i] = 0;
            }
        }
    }
}


void BlockFall::realign_grid() {
    for (int i = 0; i < rows; ++i) {
        if (is_row_empty(i)) {
            shift_rows(i);
        }
    }
}

void BlockFall::shift_rows(int emptyRow) {
    int i = emptyRow;
    for (int j = i - 1; j >= 0; --j) {
        for (int k = 0; k < cols; ++k) {
            grid.at(j + 1).at(k) = grid.at(j).at(k);
        }
    }
    for (int & j : grid[0]) {
        j = 0;
    }
}

void BlockFall::switch_gravity() {
    gravity_mode_on = !gravity_mode_on;
    remove_block();
    update_grid(false);
}

char BlockFall::deploy_block() {
    if(active_rotation == nullptr) {
        game_over = true;
        std::cout << "End of blocks." << "\n";
        return 'w';
    }
    if(!can_be_placed(active_rotation)) {
        game_over = true;
        std::cout << "Block couldn't be deployed!" << "\n";
        return 'l';
    }
    place_block(active_rotation);
    return 'c';
}

void BlockFall::add_drop_points(int rowToDrop) {
    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[0].size(); ++j) {
            if(active_rotation->shape[i][j])
                current_score += rowToDrop;
        }
    }
}

BlockFall::~BlockFall() {
    deleteList(initial_block);
}

void deleteList(Block* head) {
    if(head == nullptr) {
        return;
    }
    deleteList(head->next_block);
    head->left_rotation->right_rotation = nullptr;
    deleteInnerList(head);
}

void deleteInnerList(Block* head) {
    if(head == nullptr) {
        return;
    }
    deleteInnerList(head->right_rotation);
    delete head;
}

Block* rotate90CW(Block* block) {
    size_t row = block->shape.size();
    size_t col = block->shape[0].size();
    std::vector<std::vector<bool>> shape;
    for (int j = 0; j < col; ++j) {
        std::vector<bool> rotRow;
        for (int i = row - 1; i >= 0; i--) {
            rotRow.push_back(block->shape[i][j]);
        }
        shape.push_back(rotRow);
    }
    Block* rotation = new Block();
    rotation->shape = shape;
    return rotation;
}

void createRotations(Block* head) {
    Block* temp = head;
    std::vector<Block*> garbageRotations;
    for (int i = 0; i < 3; ++i) {
        temp = rotate90CW(temp);
        if(isNewRotation(head, temp)) {
            addRotation(head, temp);
        } else {
            garbageRotations.push_back(temp);
        }
    }
    for (auto &garbageRotation : garbageRotations) {
        delete garbageRotation;
    }
    Block* tail = head;
    while(tail->right_rotation != nullptr) {
        tail = tail->right_rotation;
    }
    head->left_rotation = tail;
    tail->right_rotation = head;
}

void addRotation(Block* head, Block* rotation) {
    Block* temp = head;
    while(temp->right_rotation != nullptr) {
        temp = temp->right_rotation;
    }
    temp->right_rotation = rotation;
    rotation->left_rotation = temp;
    rotation->next_block = head->next_block;
}

bool isNewRotation(Block* head, Block* rotation) {
    for(Block* block = head; block != nullptr; block = block->right_rotation) {
        if(*block == *rotation)
            return false;
    }
    return true;
}

std::vector<std::vector<std::vector<bool>>> getShapes(std::ifstream &ifs) {
    std::string str;
    std::vector<std::vector<std::vector<bool>>> shapes;
    std::vector<std::vector<bool>> shape;
    while(ifs >> str) {
        std::vector<bool> row;
        for (char i : str) {
            switch (i) {
                case '1':
                    row.push_back(true);
                    break;
                case '0':
                    row.push_back(false);
                    break;
                case '[':
                    shapes.push_back(shape);
                    shape.clear();
                    break;
                default:
                    continue;
            }
        }
        shape.push_back(row);
    }
    shapes.push_back(shape);
    shapes.erase(shapes.begin());
    return shapes;
}