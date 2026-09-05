#include "kwic.h"

#include <algorithm>
#include <iterator>
#include <vector>
#include <fmt/color.h>
#include <fmt/format.h>

#include "details.h"

namespace
{
struct shift_entry
{
    size_t line_index;
    size_t word_index;
};

std::string g_characters;
std::vector<std::vector<std::string_view>> g_words; // words per line
std::vector<shift_entry> g_index;
std::vector<shift_entry> g_alpha_index;

auto input(const std::string &p_path)
{
    g_characters = details::read_file(p_path);
}

auto circular_shift()
{
    size_t line_start = 0U;
    for (size_t i = 0U; i <= g_characters.length(); ++i) {
        if (i == g_characters.length() || g_characters.at(i) == '\n') {
            const auto len = i - line_start;
            const auto line = std::string_view(&g_characters.at(line_start), len);
            g_words.push_back(details::tokenize_line(line));

            const auto line_index = g_words.size() - 1;
            for (size_t word_index = 0U; word_index < g_words.at(line_index).size(); ++word_index) {
                g_index.emplace_back(line_index, word_index);
            }
            line_start = i + 1;
        }
    }
}

auto alphabetize()
{
    g_alpha_index = g_index;
    std::ranges::sort(g_alpha_index, [](const shift_entry &p_lhs, const shift_entry &p_rhs) -> bool {
        const auto &lwords = g_words.at(p_lhs.line_index);
        const auto &rwords = g_words.at(p_rhs.line_index);
        const auto min_words_per_line = std::min(lwords.size(), rwords.size());
        for (size_t i = 0; i < min_words_per_line; ++i) {
            const auto lword = lwords.at((p_lhs.word_index + i) % lwords.size());
            const auto rword = rwords.at((p_rhs.word_index + i) % rwords.size());
            if (lword != rword) return lword < rword;
        }
        return lwords.size() < rwords.size();
    });
}

auto output(size_t p_width) -> void
{
    for (const auto &[line_index, word_index] : g_alpha_index) {
        const auto &words = g_words[line_index];
        const auto &kw = words[word_index];

        std::string right;
        for (size_t k = 1; k < words.size() && right.size() < p_width; ++k) {
            const auto &w = words[(word_index + k) % words.size()];
            if (!right.empty()) {
                right += ' ';
            }
            right += w;
        }
        if (right.size() > p_width) {
            right.resize(p_width);
        }

        std::string left;
        for (size_t k = 1; k < words.size() && left.size() < p_width; ++k) {
            const auto &w = words[(word_index + words.size() - k) % words.size()];
            if (left.empty()) {
                left = w;
            } else {
                left = fmt::format("{} {}", w, left);
            }
        }
        if (left.size() > p_width) {
            left = left.substr(left.size() - p_width);
        }

        fmt::print("{:>{}} {} {:<{}}\n",
            left, p_width,
            fmt::styled(kw, fmt::emphasis::bold),
            right, p_width);
    }
}
} // namespace

auto shared_data::kwic(const std::string &p_path, size_t p_width) -> void
{
    input(p_path);
    circular_shift();
    alphabetize();
    output(p_width);
}
