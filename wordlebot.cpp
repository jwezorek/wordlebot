#include "wordle_state.hpp"
#include <iostream>

namespace {

    constexpr int k_max_guesses = 6;

    enum class game_state {
        in_progress,
        success,
        failure
    };

}

int main()
{
    wbt::wordle_state wordlebot;
    std::string result;
    int count = 0;
    game_state state = game_state::in_progress;
    while (state == game_state::in_progress && count < k_max_guesses) {
        std::cout << "\n";
        do {
            std::string guess;
            count++;
            if (result.empty()) {
                std::cout << "wordlebot guesses '" << 
                    (guess = wordlebot.initial_guess(25)) << "'\n";
            } else {
                guess = wordlebot.guess(8);
                if (!guess.empty()) {
                    std::cout << "wordlebot guesses '" << guess << "'\n";
                } else {
                    std::cout << "wordlebot does not know the word." << guess << "\n";
                    state = game_state::failure;
                    break;
                }
            }
            std::cout << "  => ";
            std::cin >> result;
            if (result != std::string(5, wbt::k_green)) {
                wordlebot.insert(guess, result);
            } else {
                state = game_state::success;
            }
        } while (result == "x");
    }

    if (state == game_state::success) {
        std::cout << "\n  " << count << "/6\n";
    } else {
        std::cout << "\n  *\n";
    }
}

