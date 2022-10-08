#include "words.hpp"
#include <range/v3/all.hpp>
#include <iostream>
#include <string>
#include <random>
#include <array>
#include <set>
#include <map>

namespace r = ranges;
namespace rv = ranges::views;

namespace {

    constexpr char k_green = 'g';
    constexpr char k_yellow = 'y';
    constexpr char k_gray = '-';
    constexpr int k_max_guesses = 6;

    int random_number(int n) {
        static std::random_device dev;
        //std::mt19937 rng(dev());
        std::mt19937 rng(42);
        std::uniform_int_distribution<> uint_dist_n(0, n - 1);
        return uint_dist_n(rng);
    }

    std::string random_word(const std::vector<std::string>& words) {
        return words[random_number(static_cast<int>(words.size()))];
    }

    class wordle_state {
        const std::vector<std::string>& words_by_score_;
        std::array<char, 5> must_be_;
        std::array<std::set<char>, 5> must_not_be_;
        std::set<std::string> guess_history_;
        std::map<char,int> must_have_somewhere_;
        std::set<char> cant_have_anywhere_;

        bool is_valid_result(const std::string& result) {
            for (auto ch : result) {
                if (ch != k_green && ch != k_yellow && ch != k_gray) {
                    return false;
                }
            }
            return true;
        }

        bool is_valid_guess(const std::string& guess) const {

            if (guess_history_.find(guess) != guess_history_.end()) {
                return false;
            }

            for (auto letter : guess) {
                if (cant_have_anywhere_.find(letter) != cant_have_anywhere_.end()) {
                    return false;
                }
            }

            auto must_have_somewhere = must_have_somewhere_;
            
            for (auto [guess_letter, must_be_letter] : rv::zip(guess, must_be_)) {
                if (must_be_letter) {
                    if (must_be_letter != guess_letter) {
                        return false;
                    } else {
                        auto iter = must_have_somewhere.find(must_be_letter);
                        if (iter != must_have_somewhere.end() && iter->second != 0) {
                            must_have_somewhere[must_be_letter]--;
                        }
                    }
                }
            }

            for (const auto& [guess_letter, must_not_be_set] : rv::zip(guess, must_not_be_)) {
                if (must_not_be_set.find(guess_letter) != must_not_be_set.end()) {
                    return false;
                }
            }

            auto leftovers = rv::zip(guess, must_be_) |
                rv::transform(
                    [](const auto& pair)->char {
                        auto [guess_letter, must_be_letter] = pair;
                        return (guess_letter != must_be_letter) ? guess_letter : 0;
                    }
                ) | rv::remove_if(
                    [](char letter) {
                        return letter == 0;
                    }
                ) | r::to_vector;

            for (auto letter : leftovers) {
                auto iter = must_have_somewhere.find(letter);
                if (iter != must_have_somewhere.end() && iter->second > 0) {
                    iter->second--;
                }
            }

            auto remaining = r::accumulate(
                    must_have_somewhere | 
                        rv::transform(
                            [](const auto& p) { return p.second;}
                        ), 
                    0
                );
            return (remaining == 0);
        }

    public:
        wordle_state() :
            words_by_score_(wbt::word_list_by_score()),
            must_be_({0,0,0,0,0})
        {}

        std::string initial_guess(int n) {
            return words_by_score_[random_number(n)];
        }

        std::string guess(int n) const {
            auto guesses = words_by_score_ |
                rv::remove_if(
                    [this](const auto& word) {
                        return !is_valid_guess(word);
                    }
                ) |
                rv::take(n) |
                r::to_vector;

            if (guesses.empty()) {
                return {};
            }

            return random_word(guesses);
        }

        bool insert(const std::string& insertee, const std::string& result) {
            if (result == "x") {
                return true;
            }

            if (!is_valid_result(result)) {
                return false;
            }

            guess_history_.insert(insertee);

            std::map<char, int> must_have_somewhere;
            std::set<char> greens_and_yellows;
            for (auto [i, insert_letter ] : rv::enumerate(insertee)) {
                auto value = result[i];
                switch (value) {
                    case k_green:
                        // green means the ith guess letter must be 'insert_letter'
                        must_be_[i] = insert_letter;
                        greens_and_yellows.insert(insert_letter);
                        break;
                    case k_yellow:
                        // yellow means the ith letter must not be 'insert_letter' but
                        // 'insert_letter' must be used somewhere else.
                        if (must_not_be_[i].find(insert_letter) == must_not_be_[i].end()) {
                            must_not_be_[i].insert(insert_letter);
                            must_have_somewhere[insert_letter]++;
                        }
                        greens_and_yellows.insert(insert_letter);
                        break;
                    case k_gray:
                        // gray means the ith letter must not be 'insert_letter'
                        must_not_be_[i].insert(insert_letter);
                        break;
                }
            }

            // gray also means if the gray letter is not a yellow or green
            // elsewhere in this insertee then it must not be anywhere.
            for (auto letter : insertee) {
                if (greens_and_yellows.find(letter) == greens_and_yellows.end()) {
                    cant_have_anywhere_.insert(letter);
                }
            }

            // merge the letters we know must be somewhere from this word
            // into the global state.
            for (const auto [letter, count] : must_have_somewhere) {
                auto iter = must_have_somewhere_.find(letter);
                if (iter == must_have_somewhere_.end()) {
                    must_have_somewhere_[letter] = count;
                } else {
                    if (count > iter->second) {
                        iter->second = count;
                    }
                }
            }
            return true;
        }
    };

    enum class game_state {
        in_progress,
        success,
        failure
    };
}

int main()
{
    wordle_state wordlebot;
    std::string result;
    int count = 0;
    game_state state = game_state::in_progress;
    while (state == game_state::in_progress && count < k_max_guesses) {
        std::cout << "\n";
        do {
            std::string guess;
            count++;
            if (result.empty()) {
                std::cout << "wordlebot guesses '" << (guess = wordlebot.initial_guess(25)) << "'\n";
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
            if (result != std::string(5, k_green)) {
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

