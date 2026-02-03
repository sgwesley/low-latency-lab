#pragma once

#include <iostream>
#include <vector>
#include <execution>
#include <thread>

#include "utils.h"

class LotteryProcessor {
public:
    LotteryProcessor() {}

    // Aligned to avoid false sharing between threads
    struct alignas(64) Counter {
        int winners[6] = {0, 0, 0, 0, 0, 0};
    };

    void Process(
#ifdef ENABLE_SOA
        const PlayersInfo& data,
#else
        const std::vector<PlayerInfo>& data,
#endif
        const std::vector<int>& play) {
        if (!Utils::ValidatePlay(play)) {
            std::cout << "One or more of the picked numbers are not correct" << std::endl;
            return;
        }

        uint64_t pickedNumMask = 0;
        Utils::SetPlayToMask(play, pickedNumMask);
        int winnersCounter[6] = {0, 0, 0, 0, 0, 0};
#ifdef ENABLE_SOA
        size_t dataSize = data.player_id.size();
#else
        size_t dataSize = data.size();
#endif

        /* Explanation: the matching process is executed in chunks of size N divided by T, 
         * where N is the total number of plays and T is the number of available threads. 
         * Each thread performs a bitwise AND operation between the play bitmap and the picked numbers bitmap, 
         * followed by a population count to determine how many numbers match (check processRange method).
         */
        const unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency());
        std::vector<Counter> counters(numThreads);
        std::vector<std::thread> threads;
        size_t chunk = dataSize / numThreads;
        for (unsigned int t = 0; t < numThreads; ++t) {
            size_t start = t * chunk;
            size_t end = (t+1==numThreads) ? dataSize : start+chunk;
            threads.emplace_back([&, start, end, t]() {
                processRange(data, start, end, pickedNumMask, counters[t]);
            });
        }

        for (auto &th: threads) th.join();

        /* Explanation: after all threads complete their execution, their individual counters are aggregated 
         * into a final winnersCounter array to produce the overall results.
        */
        for (const auto& counter : counters) {
            for (int i = 0; i < 6; ++i) {
                winnersCounter[i] += counter.winners[i];
            }
        }

        // Output results in the format: [2 matches count] [3 matches count] [4 matches count] [5 matches count]
        std::cout << winnersCounter[2] << " " << winnersCounter[3] << " " << winnersCounter[4] << " " << winnersCounter[5] << std::endl;
    }

private:
    void processRange(
#ifdef ENABLE_SOA
        const PlayersInfo& data,
#else
        const std::vector<PlayerInfo>& data,
#endif
                      size_t start,
                      size_t end,
                      const uint64_t pickedNumMask,
                      Counter& counter) {
        for (size_t i = start; i < end; i++) {
#ifdef ENABLE_SOA
            int matches =
                __builtin_popcountll(data.play_mask[i] & pickedNumMask);

            counter.winners[matches]++;
#else
            int matches =
                __builtin_popcountll(data[i].play_mask & pickedNumMask);

            counter.winners[matches]++;
#endif
        }
    }
};