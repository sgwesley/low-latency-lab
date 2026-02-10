#include <iostream>
#include <chrono>

#include "lottery_input_reader.h"
#include "lottery_processor.h"

void readUserInput(std::vector<std::string>& words) {
    size_t wStart = -1;
    size_t wSize = 0;
    std::string line;
    
    std::cout << "Please enter five numbers between 1 and 60 (separated by spaces):" << std::endl;
    while (words.size() < 5 && std::getline(std::cin, line)) {
        wStart = -1;
        for (auto i=0; i<line.size(); i++) {
            wSize = i - wStart;
            if (std::isspace(line[i])) {
                // dealing with spaces at the beginning and at the end, and
		        // also with multiple spaces between words
                if ( wSize > 1) { 
                    words.push_back(line.substr(wStart+1, wSize-1));
                }
                
                wStart = i;
            } else if (i == line.size() - 1) {
                words.push_back(line.substr(wStart+1, wSize));
            } 
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    LotteryInputReader reader(argv[1]);
    LotteryProcessor processor;
    std::vector<std::string> userInput;
    std::vector<int> play;

    if (!reader.Read()) {
        std::cout << "Failed to read input file" << std::endl;
        return 1;
    }

    readUserInput(userInput);
    if (userInput.size() > 5) {
        std::cout << "Please provide exactly five numbers." << std::endl;
        return 1;
    }

    
    for (const auto& input : userInput) {
        try {
            int num = std::stoi(input);
            play.push_back(num);
        } catch (const std::invalid_argument&) {
            std::cout << "Invalid number: " << input << std::endl;
            return 1;
        }
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    processor.Process(reader.GetData(), play);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "(elapsed time: " << elapsed_ms.count() << " us)" << std::endl;
    
    return 0;
}
