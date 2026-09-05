#include "details.h"

#include <algorithm>
#include <cassert>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <fmt/format.h>

namespace details
{
auto read_file(const std::string &p_path) -> std::string
{
    std::ifstream file(p_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error(fmt::format("could not open file: {}", p_path));
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

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
        const auto word_length = std::distance(word_start, word_end);
        assert(word_length > 0);
        std::string_view word(word_start, static_cast<size_t>(word_length));
        words.push_back(word);
        it = word_end;
    }

    return words;
}
} // namespace details