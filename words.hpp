#include <vector>
#include <string>
#include <unordered_map>
#include <string>

namespace wbt {
    const std::vector<std::string>& word_list_by_score();
    const std::vector<std::string>& word_list_by_freqency();
    double word_frequency(const std::string& word);
}