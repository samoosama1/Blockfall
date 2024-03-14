#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).

    // Set to head if applicable
    if(head_leaderboard_entry == nullptr) {
        head_leaderboard_entry = new_entry;
        size++;
        check_overflow();
        return;
    }
    if(new_entry->compare(*head_leaderboard_entry) == 1) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
        size++;
        check_overflow();
        return;
    }

    // Insert middle
    LeaderboardEntry* temp = head_leaderboard_entry;
    while(temp->next_leaderboard_entry != nullptr) {
        if(new_entry->compare(*temp->next_leaderboard_entry) == 1) {
            new_entry->next_leaderboard_entry = temp->next_leaderboard_entry;
            temp->next_leaderboard_entry = new_entry;
            size++;
            check_overflow();
            return;
        }
        temp = temp->next_leaderboard_entry;
    }
    // If not inserted in the middle append to tail
    if(size == MAX_LEADERBOARD_SIZE)
        return;
    temp->next_leaderboard_entry = new_entry;
}

void Leaderboard::check_overflow() {
    if(size > MAX_LEADERBOARD_SIZE) {
        pop_back();
        size--;
    }
}

void Leaderboard::pop_back() {
    LeaderboardEntry* temp = head_leaderboard_entry;
    while (temp->next_leaderboard_entry->next_leaderboard_entry != nullptr) {
        temp = temp->next_leaderboard_entry;
    }
    delete temp->next_leaderboard_entry;
    temp->next_leaderboard_entry = nullptr;
}


void Leaderboard::write_to_file(const string& filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
    std::ofstream ofs{filename};
    auto temp = head_leaderboard_entry;
    while(temp != nullptr) {
        ofs << temp->score << " " << temp->last_played << " " << temp->player_name;
        temp = temp->next_leaderboard_entry;
        if(temp != nullptr)
            ofs << "\n";
    }
}

void Leaderboard::read_from_file(const string& filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.
    std::ifstream ifs{filename};
    if (!ifs.is_open()) {
        return;
    }
    std::string line;
    while (getline(ifs, line)){
        if(line.empty())
            continue;
        unsigned long score;
        time_t time;
        std::string name;
        std::stringstream ss{line};
        ss >> score >> time >> name;
        auto* newEntry = new LeaderboardEntry{score, time, name};
        insert_new_entry(newEntry);
    }
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    std::cout << "Leaderboard\n";
    std::cout << "-----------\n";
    int i = 1;
    for (auto temp = head_leaderboard_entry; temp != nullptr; temp = temp->next_leaderboard_entry) {
        //std::cout << i << ". " << temp->player_name << " " << temp->score << " " << time_t_to_string(temp->last_played) << "\n";
        i++;
    }
    std::cout << "\n";
}

std::string Leaderboard::time_t_to_string(time_t time) {
    std::unordered_map<std::string, std::string> mp;
    mp["Jan"] = "01";
    mp["Feb"] = "02";
    mp["Mar"] = "03";
    mp["Apr"] = "04";
    mp["May"] = "05";
    mp["Jun"] = "06";
    mp["Jul"] = "07";
    mp["Aug"] = "08";
    mp["Sep"] = "09";
    mp["Oct"] = "10";
    mp["Nov"] = "11";
    mp["Dec"] = "12";
    char* chArr = asctime(localtime(&time));
    string strToFormat{chArr};
    stringstream ss{strToFormat};
    string dayName;
    string month;
    string day;
    string _time;
    string year;
    ss >> dayName >> month >> day >> _time >> year;
    string timeStamp = _time + "/" + day + "." + mp.at(month) + "." + year;
    return timeStamp;
}

void Leaderboard::delete_list(LeaderboardEntry* node) {
    if(node == nullptr)
        return;
    delete_list(node->next_leaderboard_entry);
    delete node;
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    delete_list(head_leaderboard_entry);
}



