#include "wordle_state.hpp"
#include "words.hpp"
#include <range/v3/all.hpp>
#include <random>
#include <iostream>

/*------------------------------------------------------------------------------------------------*/

namespace r = ranges;
namespace rv = ranges::views;

namespace {

    int random_number(int n) {
        static std::random_device dev;
        std::mt19937 rng(dev());
        //std::mt19937 rng(42);
        std::uniform_int_distribution<> uint_dist_n(0, n - 1);
        return uint_dist_n(rng);
    }

    std::string random_word(const std::vector<std::string>& words) {
        return words[random_number(static_cast<int>(words.size()))];
    }

}

bool wbt::wordle_state::is_valid_guess(const std::string& guess) const {

    // if it was already used it's not a valid guess
    if (guess_history_.find(guess) != guess_history_.end()) {
        return false;
    }

    // if it contains a letter known not to be in the hidden word it is not a valid guess.
    for (auto letter : guess) {
        if (cant_have_anywhere_.find(letter) != cant_have_anywhere_.end()) {
            return false;
        }
    }

    // all the known green letters must be fullfilled.
    auto must_have_somewhere = must_have_somewhere_;
    for (auto [guess_letter, must_be_letter] : rv::zip(guess, must_be_)) {
        if (must_be_letter && must_be_letter != guess_letter) {
            return false;
        }
    }

    // it can't have a guess letter that yielded yellow in some location in the same
    // location where it is known to be yellow.
    for (const auto& [guess_letter, must_not_be_set] : rv::zip(guess, must_not_be_)) {
        if (must_not_be_set.find(guess_letter) != must_not_be_set.end()) {
            return false;
        }
    }

    // do the letters that don't fulfill green positions fulfill all of the known
    // yellow letters?
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
        must_have_somewhere | rv::transform( [](const auto& p) { return p.second; }),
        0
    );
    return (remaining == 0);
}

wbt::wordle_state::wordle_state(int threshold) :
    words_by_score_(wbt::word_list_by_score()),
    words_by_freq_(wbt::word_list_by_freqency()),
    must_be_({ 0,0,0,0,0 }),
    freq_vs_score_threshold_(threshold)
{}

int wbt::wordle_state::current_score() const {
    auto must_have_somewhere_counts = must_have_somewhere_ |
        rv::transform([](const auto& p) {return p.second; });
    auto must_be_counts = must_be_ | rv::transform([](char ch) {return (ch > 0) ? 1 : 0; });

    int yellow_count = r::accumulate(must_have_somewhere_counts, 0);
    int green_count = r::accumulate(
        must_be_counts,
        0
    );
    return 2 * green_count + yellow_count;
}

std::string wbt::wordle_state::initial_guess(int n) {
    return words_by_score_[random_number(n)];
}

std::string wbt::wordle_state::guess(int n) const {
    bool use_freq_rank = valid_words_remaining() < freq_vs_score_threshold_;
    const std::vector<std::string>* in_play_word_list = 
        use_freq_rank ? &words_by_freq_ : &words_by_score_;
    /*
    if (use_freq_rank) {
        std::cout << "using words by frequency\n";
    } else {
        std::cout << "using words by score\n";
    }
    */
    auto guesses = *in_play_word_list |
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

     if (guesses.size() < n) {
         return guesses.front();
     }

     if (use_freq_rank) {
         // don't use a zero frequency word unless we have to...
         bool has_nonzero_freq_word = r::find_if(
                 guesses,
                 [](const auto& word) {
                     return word_frequency(word) > 0.0;
                 }
             ) != guesses.end();
         if (has_nonzero_freq_word) {
             guesses = guesses |
                 rv::remove_if(
                     [](const auto& word) {
                         return word_frequency(word) == 0.0;
                     }
             ) | r::to_vector;
         }
     }

     return random_word(guesses);
}

bool wbt::wordle_state::insert(const std::string& insertee, const std::string& result) {

    guess_history_.insert(insertee);

    std::map<char, int> must_have_somewhere;
    std::set<char> greens_and_yellows;
    for (auto [i, insert_letter] : rv::enumerate(insertee)) {
        auto value = result[i];
        switch (value) {
            case k_green: {
                    // green means the ith guess letter must be 'insert_letter'
                    must_be_[i] = insert_letter;

                    // remove a yellow from global state if there is one this green fulfills.
                    auto iter = must_have_somewhere_.find(insert_letter);
                    if (iter != must_have_somewhere_.end() && iter->second > 0) {
                        iter->second--;
                    }
                    // maintain a set of greens and yellow just in this word...
                    greens_and_yellows.insert(insert_letter);
                }   
                break;
            case k_yellow: {
                    // yellow means the ith letter must not be 'insert_letter' but
                    // 'insert_letter' must be used somewhere else.
                    if (must_not_be_[i].find(insert_letter) == must_not_be_[i].end()) {
                        must_not_be_[i].insert(insert_letter);
                        must_have_somewhere[insert_letter]++;
                    }
                    // maintain a set of greens and yellow just in this word...
                    greens_and_yellows.insert(insert_letter);
                }
                break;
            case k_gray: {
                    // gray means the ith letter must not be 'insert_letter'
                    must_not_be_[i].insert(insert_letter);
                }
                break;
        }
    }

    // gray also means if the gray letter is not a yellow or green
    // elsewhere in insertee then it must not be anywhere.
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

int wbt::wordle_state::valid_words_remaining() const {
    return static_cast<int>(
        r::distance(
            words_by_score_ |
            rv::remove_if(
                [this](const std::string& word)->bool {
                    return !is_valid_guess(word);
                }
            )
        )
    );
}

bool wbt::is_valid_result_string(const std::string& result) {
    if (result.size() != 5) {
        return false;
    }

    for (auto ch : result) {
        if (ch != k_green && ch != k_yellow && ch != k_gray) {
            return false;
        }
    }

    return true;
}