#include "LeaderboardEntry.h"

LeaderboardEntry::LeaderboardEntry(unsigned long score,
                                   time_t lastPlayed,
                                   const string &playerName) :
                                   score(score),
                                   last_played(lastPlayed),
                                   player_name(playerName) {}

int LeaderboardEntry::compare(LeaderboardEntry &other) {
    if(score > other.score) return 1;
    if(score == other.score) {
        if(last_played < other.last_played)
            return 1;
    }
    return -1;
}


