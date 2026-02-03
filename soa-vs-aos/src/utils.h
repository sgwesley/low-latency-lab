#pragma once

#include <vector>

struct PlayerInfo {
    uint64_t player_id = 0; 
    uint64_t play_mask; // bits 0..59 represent numbers 1..60
};

// Structure of Arrays (SoA) representation for PlayerInfo
struct PlayersInfo {
    std::vector<uint64_t> player_id;
    std::vector<uint64_t> play_mask; // bits 0..59 represent numbers 1..60

};

class Utils {
public:
    /*
     * Ensures that the play, represented as a vector, contains exactly five values
     * and that all values are within the valid range of 1 to 90 inclusive.
     */
    static bool ValidatePlay(const std::vector<int>& play) {
        if (play.size() != 5) {
            return false;
        }

        for (int number : play) {
            if (number < MinNumber || number > MaxNumber) {
                return false;
            }
        }

        return true;
    }

    /*
     * Transforms the input vector into a bitmap (mask), mapping each number N
     * to the corresponding bit position representing N.
     */
    static void SetPlayToMask(const std::vector<int>& play, uint64_t& mask) {
        mask = 0;

        for (int number : play) {
            mask |= (1ULL << (number));
        }
    }

private:
    static constexpr int MinNumber = 1;
    static constexpr int MaxNumber = 60;
};