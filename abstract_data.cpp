#include "kwic.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <fmt/color.h>
#include <fmt/format.h>

#include "details.h"

namespace
{
class characters
{
    std::string m_text{};
public:
    explicit characters(std::string_view p_text) : m_text{p_text} {}

    [[nodiscard]] auto getchar(size_t p_index) const -> std::string::value_type
    {
        return m_text.at(p_index);
    }

    [[nodiscard]] auto getview(size_t p_start, size_t p_length) const -> std::string_view
    {
        return std::string_view{m_text}.substr(p_start, p_length);
    }

    [[nodiscard]] auto getlength() const -> size_t
    {
        return m_text.length();
    }
};

struct shift_entry
{
    size_t line_index;
    size_t word_index;
};

class circular_shift
{
    const characters &m_characters; // non-owning
    std::vector<std::vector<std::string_view>> m_words; // words per line
    std::vector<shift_entry> m_index{};
public:
    explicit circular_shift(const characters &p_characters)
        : m_characters{p_characters}
    {
        size_t line_start = 0U;
        for (size_t i = 0U; i <= m_characters.getlength(); ++i) {
            if (i == m_characters.getlength() || m_characters.getchar(i) == '\n') {
                const auto len = i - line_start;
                const auto line = m_characters.getview(line_start, len);
                m_words.push_back(details::tokenize_line(line));

                const auto line_index = m_words.size() - 1;
                for (size_t word_index = 0U; word_index < m_words.at(line_index).size(); ++word_index) {
                    m_index.emplace_back(line_index, word_index);
                }
                line_start = i + 1;
            }
        }
    }

    [[nodiscard]] auto getwords(size_t p_line_index) const -> const std::vector<std::string_view> &
    {
        return m_words.at(p_line_index);
    }

    [[nodiscard]] auto getindex() const -> const std::vector<shift_entry> &
    {
        return m_index;
    }
};

class alpha_shift
{
    const circular_shift &m_circ_shift; // non-owning
    std::vector<shift_entry> m_alpha_index;
public:
    explicit alpha_shift(const circular_shift &p_circ_shift)
        : m_circ_shift{p_circ_shift}
    {
        m_alpha_index = m_circ_shift.getindex();
        std::ranges::sort(m_alpha_index, [this](const shift_entry &p_lhs, const shift_entry &p_rhs) -> bool {
            const auto &lwords = m_circ_shift.getwords(p_lhs.line_index);
            const auto &rwords = m_circ_shift.getwords(p_rhs.line_index);
            const auto min_words_per_line = std::min(lwords.size(), rwords.size());
            for (size_t i = 0; i < min_words_per_line; ++i) {
                const auto lword = lwords.at((p_lhs.word_index + i) % lwords.size());
                const auto rword = rwords.at((p_rhs.word_index + i) % rwords.size());
                if (lword != rword) return lword < rword;
            }
            return lwords.size() < rwords.size();
        });
    }

    [[nodiscard]] auto getindex() const -> const std::vector<shift_entry> &
    {
        return m_alpha_index;
    }
};

auto input(const std::string &p_path) -> characters
{
    return characters{details::read_file(p_path)};
}

auto output(const circular_shift &p_circ_shift, const alpha_shift &p_alpha_shift, size_t p_width) -> void
{
    for (const auto &[line_index, word_index] : p_alpha_shift.getindex()) {
        const auto &words = p_circ_shift.getwords(line_index);
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

auto abstract_data::kwic(const std::string &p_path, size_t p_width) -> void
{
    const auto characters = input(p_path);
    const circular_shift circ_shift{characters};
    const alpha_shift alpha_shift{circ_shift};
    output(circ_shift, alpha_shift, p_width);
}
