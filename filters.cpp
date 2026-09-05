#include "kwic.h"

#include <algorithm>
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

struct shifted_lines
{
    std::vector<std::vector<std::string_view>> words;
    std::vector<shift_entry> index;
};

auto input(const std::string_view p_text)
{
    return p_text;
}

auto circular_shift(const std::string_view p_text)
{
    std::vector<std::vector<std::string_view>> words; // words per line
    std::vector<shift_entry> index;

    size_t line_start = 0U;
    for (size_t i = 0U; i <= p_text.length(); ++i) {
        if (i == p_text.length() || p_text.at(i) == '\n') {
            const auto len = i - line_start;
            const auto line = p_text.substr(line_start, len);
            words.push_back(details::tokenize_line(line));

            const auto line_index = words.size() - 1;
            for (size_t word_index = 0U; word_index < words.at(line_index).size(); ++word_index) {
                index.emplace_back(line_index, word_index);
            }
            line_start = i + 1;
        }
    }

    return shifted_lines{words, index};
}

auto alphabetize(const shifted_lines &p_shifted)
{
    std::vector<shift_entry> alpha_index = p_shifted.index;
    std::ranges::sort(alpha_index, [&p_shifted](const shift_entry &p_lhs, const shift_entry &p_rhs) -> bool {
        const auto &lwords = p_shifted.words.at(p_lhs.line_index);
        const auto &rwords = p_shifted.words.at(p_rhs.line_index);
        const auto min_words_per_line = std::min(lwords.size(), rwords.size());
        for (size_t i = 0; i < min_words_per_line; ++i) {
            const auto lword = lwords.at((p_lhs.word_index + i) % lwords.size());
            const auto rword = rwords.at((p_rhs.word_index + i) % rwords.size());
            if (lword != rword) return lword < rword;
        }
        return lwords.size() < rwords.size();
    });

    return alpha_index;
}

auto output(const shifted_lines &p_shifted, const std::vector<shift_entry> &p_alpha_index, size_t p_width) -> void
{
    for (const auto &[line_index, word_index] : p_alpha_index) {
        const auto &words = p_shifted.words[line_index];
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

auto filters::kwic(const std::string_view p_text) -> void
{
    const auto characters = input(p_text);
    const auto shifted = circular_shift(characters);
    const auto alpha_index = alphabetize(shifted);
    output(shifted, alpha_index, 40);
}
