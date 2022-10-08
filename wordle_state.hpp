#pragma once

#include <vector>
#include <array>
#include <string>
#include <set>
#include <map>

namespace wbt {

    constexpr char k_green = 'g';
    constexpr char k_yellow = 'y';
    constexpr char k_gray = '-';

    class wordle_state {
        const std::vector<std::string>& words_by_score_;
        std::array<char, 5> must_be_;
        std::array<std::set<char>, 5> must_not_be_;
        std::set<std::string> guess_history_;
        std::map<char, int> must_have_somewhere_;
        std::set<char> cant_have_anywhere_;

        bool is_valid_result(const std::string& result) const;
        bool is_valid_guess(const std::string& guess) const;

    public:
        wordle_state();
        std::string initial_guess(int n);
        std::string guess(int n) const;
        bool insert(const std::string& insertee, const std::string& result);
    };

}