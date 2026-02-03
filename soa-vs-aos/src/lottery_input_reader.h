#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "utils.h"

class LotteryInputReader {
public:
    LotteryInputReader(const std::string& filename) : m_fileStream(filename) {}

    ~LotteryInputReader() {
        if (m_fileStream.is_open()) {
            m_fileStream.close();
        }
    }

    bool Read() {
        if (!m_fileStream.is_open()) {
            std::cout << "Error opening file" << std::endl;
            return false;
        }

        std::string line;
        uint64_t lineNumber = 0;

        while (std::getline(m_fileStream, line)) {
            std::istringstream iss(line);
            std::vector<int> row;
            int value;

            while (iss >> value) {
                row.emplace_back(value);
            }

            if (Utils::ValidatePlay(row)) {
                PlayerInfo play;
                play.player_id = lineNumber + 1; // Assign a unique player ID
                Utils::SetPlayToMask(row, play.play_mask);
#ifdef ENABLE_SOA
                m_data.player_id.emplace_back(play.player_id);
                m_data.play_mask.emplace_back(play.play_mask);
#else
                m_data.emplace_back(play);
#endif
            } else {
                std::cout << "Invalid play: " << line << ", ignoring it" << std::endl;
            }

            lineNumber++;
        }

#ifdef ENABLE_SOA
        if (m_data.player_id.empty()) {
#else
        if (m_data.empty()) {
#endif
            std::cout << "No data read from file" << std::endl;
            return false;
        }

        std::cout << "READY" << std::endl;
        return true;
    }

#ifdef ENABLE_SOA
    const PlayersInfo&
#else
    const std::vector<PlayerInfo>& 
#endif
    GetData() const {
        return m_data;
    }

private:
    std::ifstream m_fileStream;
#ifdef ENABLE_SOA
    PlayersInfo m_data;
#else
    std::vector<PlayerInfo> m_data;
#endif
};