#include <vector>
#include <string>
#include <unordered_map>
#include <string>

namespace wbt {
    const std::vector<std::string>& word_list_alphabetic();
    const std::vector<std::string>& word_list_by_score();
    const std::vector<std::string>& word_list_by_freqency();
    const std::unordered_map<std::string, double>& word_score_table();
}