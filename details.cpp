#include "details.h"

#include <algorithm>
#include <locale>

namespace details
{
auto tokenize_line(const std::string_view p_line) -> std::vector<std::string_view>
{
    const auto is_letter = [](const char p_c) { return isalpha(p_c) != 0; };
    const auto is_not_letter = [](const char p_c) { return isalpha(p_c) == 0; };

    std::vector<std::string_view> words;

    auto *it = p_line.cbegin();
    while (it != p_line.cend())
    {
        const auto *word_start = std::find_if(it, p_line.cend(), is_letter);
        if (word_start == p_line.cend()) {
            break;
        }
        const auto *word_end = std::find_if(word_start, p_line.cend(),  is_not_letter);
        std::string_view word(word_start, std::distance(word_start, word_end));
        words.push_back(word);
        it = word_end;
    }

    return words;
}
} // namespace details