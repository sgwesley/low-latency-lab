#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <immintrin.h>

#include "utils.h"

class LotteryProcessor {
public:
    LotteryProcessor() {}

    // Aligned to avoid false sharing between threads
    struct alignas(64) Counter {
        int winners[6] = {0, 0, 0, 0, 0, 0};
    };

    void Process(const PlayersInfo& data, const std::vector<int>& play) {
        if (!Utils::ValidatePlay(play)) {
            std::cout << "One or more of the picked numbers are not correct" << std::endl;
            return;
        }

        uint64_t pickedNumMask = 0;
        Utils::SetPlayToMask(play, pickedNumMask);
        int winnersCounter[6] = {0, 0, 0, 0, 0, 0};
        size_t dataSize = data.player_id.size();

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
    void processRange(const PlayersInfo& data,
                      size_t start,
                      size_t end,
                      const uint64_t pickedNumMask,
                      Counter& counter) {
        // AVX2: process 4 x 64-bit masks in parallel
        const size_t vectorSize = 4;
        __m256i pickedVec = _mm256_set1_epi64x(pickedNumMask);
        
        size_t i = start;
        
        // Process in chunks of 4 with AVX2
        for (; i + vectorSize <= end; i += vectorSize) {
            __m256i plays = _mm256_loadu_si256((__m256i*)&data.play_mask[i]);
            __m256i result = _mm256_and_si256(plays, pickedVec);
            
            // Extract and popcount each 64-bit lane
            uint64_t lane0 = (uint64_t)_mm256_extract_epi64(result, 0);
            uint64_t lane1 = (uint64_t)_mm256_extract_epi64(result, 1);
            uint64_t lane2 = (uint64_t)_mm256_extract_epi64(result, 2);
            uint64_t lane3 = (uint64_t)_mm256_extract_epi64(result, 3);
            
            counter.winners[__builtin_popcountll(lane0)]++;
            counter.winners[__builtin_popcountll(lane1)]++;
            counter.winners[__builtin_popcountll(lane2)]++;
            counter.winners[__builtin_popcountll(lane3)]++;
        }
        
        // Handle remainder with scalar code
        for (; i < end; i++) {
            int matches =
                __builtin_popcountll(data.play_mask[i] & pickedNumMask);

            counter.winners[matches]++;
        }
    }
};
