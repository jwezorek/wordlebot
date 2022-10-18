#include "wordle_state.hpp"
#include <iostream>

namespace {

    constexpr char k_quit = 'q';
    constexpr char k_unknown_word = 'x';
    constexpr int k_max_guesses = 6;
    constexpr int k_score_threshold = 3;
    constexpr int k_initial_guesses = 10;
    constexpr int k_guesses = 3;

    const auto k_quit_str = std::string(1, k_quit);
    const auto k_unknown_word_str = std::string(1, k_unknown_word);
    const auto k_win_result = std::string(5, wbt::k_green);

    enum class game_state {
        in_progress,
        success,
        failure
    };

    bool play_wordle() {
        
        wbt::wordle_state wordlebot(k_score_threshold);
        std::string result;
        int count = 0;
        game_state state = game_state::in_progress;

        while (state == game_state::in_progress && count < k_max_guesses) {
            std::cout << "\n";
            do {
                std::string guess;
                count++;
                if (result.empty()) {
                    guess = wordlebot.initial_guess(k_initial_guesses);
                    std::cout << "wordlebot guesses '" << guess << "'\n";
                } else {
                    guess = wordlebot.guess(k_guesses);
                    if (!guess.empty()) {
                        std::cout << "wordlebot guesses '" << guess << "'\n";
                    } else {
                        std::cout << "wordlebot does not know the word." << guess << "\n";
                        state = game_state::failure;
                        break;
                    }
                }
          
                do {
                    std::cout << "  => ";
                    std::cin >> result;
                } while (
                    !wbt::is_valid_result_string(result) &&
                    result != k_quit_str &&
                    result != k_unknown_word_str
                );

                if (result == k_quit_str) {
                    return false;
                }

                if (result != k_win_result) {
                    wordlebot.insert(guess, result);
                } else {
                    state = game_state::success;
                }

            } while (result == k_unknown_word_str);
        }

        if (state == game_state::success) {
            std::cout << "\n  " << count << "/6\n";
        } else {
            std::cout << "\n  *\n";
        }

        return true;
    }
}

int main()
{
    bool continue_playing = true;
    while (continue_playing) {
        continue_playing = play_wordle();
        std::cout << "\n-----------\n";
    } 

    return 0;
}

