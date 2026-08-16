#include "kwic.h"

#include <algorithm>
#include <functional>
#include <list>
#include <optional>
#include <set>
#include <vector>
#include "fmt/color.h"
#include <fmt/format.h>

#if 0
namespace
{
constexpr auto kSideWidth() {
    return 20U;
}

class characters
{
    std::string m_text{};
public:
    auto setchar(std::string::value_type p_char) -> void
    {
        m_text += p_char;
    }

    [[nodiscard]] auto getchar(size_t p_index) const -> std::optional<std::string::value_type>
    {
        try {
            return m_text.at(p_index);
        } catch (const std::out_of_range &) {
            return {};
        }
        __builtin_unreachable();
    }

    [[nodiscard]] auto getword(size_t p_index, size_t p_length) const -> std::optional<std::string_view>
    {
        try {
            return {m_text.substr(p_index, p_length)};
        } catch (const std::out_of_range &) {
            return {};
        }
        __builtin_unreachable();
    }

    [[nodiscard]] auto getlength() const -> size_t
    {
        return m_text.length();
    }
};

struct index_entry
{
    size_t start_col;
    size_t end_col;
    size_t position;
    size_t length;
};

class circular_shift
{
    std::vector<index_entry> m_index{};
    const characters &m_characters; // non-owning
public:
    explicit circular_shift(const characters &p_characters)
        : m_characters{p_characters}
    {
        enum class state {scanning, in_word} curr_state{state::scanning};
        size_t word_pos = 0;
        size_t i = 0;
        while (auto c = p_characters.getchar(i))  {
            if (iscntrl(*c) != 0 || isspace(*c) != 0 || ispunct(*c)) {
                if (curr_state == state::in_word) {
                    const auto word_len = i - word_pos;
                    const auto start_col = word_pos >= kSideWidth() ? word_pos - kSideWidth() : word_pos;
                    const auto end_col = p_characters.getlength() - i > kSideWidth() ? i + kSideWidth() : p_characters.getlength();
                    m_index.emplace_back(start_col, end_col, word_pos, word_len);
                    curr_state = state::scanning;
                } else {
                    // continue scanning
                }
            } else if (isprint(*c) != 0) {
                if (curr_state == state::scanning) {
                    word_pos = i;
                    curr_state = state::in_word;
                } else {
                    // ignore
                }
            }
            ++i;
        }
        if (curr_state == state::in_word) {
            const auto word_len = i - word_pos;
            const auto start_col = word_pos >= kSideWidth() ? word_pos - kSideWidth() : word_pos;
            const auto end_col = p_characters.getlength() - i > kSideWidth() ? i + kSideWidth() : p_characters.getlength();
            m_index.emplace_back(start_col, end_col, word_pos, word_len);
        }
    }

    [[nodiscard]] auto getword(size_t p_index) const -> std::optional<std::string_view>
    {
        try {
            auto [start_col, end_col, position, length] = m_index.at(p_index);
            return {m_characters.getword(position, length)};
        } catch (const std::out_of_range &) {
            return {};
        }
        __builtin_unreachable();
    }

    [[nodiscard]] auto getindex(size_t p_index) const -> std::optional<index_entry>
    {
        try {
            return m_index.at(p_index);
        } catch (const std::out_of_range &) {
            return {};
        }
        __builtin_unreachable();
    }
};

class alpha_shift
{
    const circular_shift &m_circ_shift; // non-owning

    std::function<bool (const std::string &p_lhs, const std::string &p_rhs)> m_less;

    std::multiset<std::string, decltype(m_less)> m_alpha_index;
    std::list<>
public:
    explicit alpha_shift(const circular_shift &p_circ_shift)
        : m_circ_shift{p_circ_shift}
        , m_less{[this](const size_t &p_lhs, const size_t &p_rhs) -> bool {
            try {
                return m_circ_shift.getword(p_lhs).value() < m_circ_shift.getword(p_rhs).value();
            } catch (const std::bad_optional_access &) {
                return false;
            }
            return false;
        }}
        , m_alpha_index{m_less}
    {
    }

    auto alphabetize()
    {
        for (size_t i = 0U; const auto w = m_circ_shift.getword(i); ++i) {
            m_alpha_index.insert(p_entry);
        }
    }

};

auto input(const std::string_view p_text) -> characters
{
    characters characters{};
    std::ranges::for_each(p_text, [&characters](const auto p_c) {
        characters.setchar(p_c);
    });
    return characters;
}

template <class AlphaIndexT>
auto output(const AlphaIndexT &p_alpha_index, const std::string_view p_input)
{
    for (const auto [start_col, end_col, position, length] : p_alpha_index) {
        fmt::print("{:>{}}{:}{:<{}}\n",
            p_input.substr(start_col, position - start_col),
            kSideWidth(),
            fmt::styled(p_input.substr(position, length), fmt::emphasis::bold),
            p_input.substr(position + length, end_col - (position + length)),
            kSideWidth());
    }
}
} // namespace

auto abstract_data::kwic(const std::string_view p_text) -> void
{
    const auto characters = input(p_text);
    circular_shift circ_shift{characters};
    alpha_shift alpha_shift{circ_shift};

    alpha_shift.alphabetize();

    const auto alpha_index = alphabetize(index, characters);
    output(alpha_index, characters);
}

#endif