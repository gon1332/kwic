#include "kwic.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <fmt/color.h>
#include <fmt/format.h>

#include "details.h"

namespace
{
template <typename Payload>
class subject;

template <typename Payload>
class observer
{
public:
    using subject_type = subject<Payload>;
    using payload_type = Payload;

    explicit observer(subject_type &p_subject);
    virtual ~observer();

    virtual void update(subject_type &p_subject, const payload_type &p_payload) = 0;

    observer(const observer &) = delete;
    auto operator=(const observer &) -> observer & = delete;
    observer(observer &&) = delete;
    auto operator=(observer &&) -> observer & = delete;

private:
    subject_type &m_subject;
};

template <typename Payload>
class subject final
{
    using ref_observer = std::reference_wrapper<observer<Payload>>;
    std::vector<ref_observer> m_observers;

public:
    auto attach(ref_observer p_observer) -> void
    {
        m_observers.push_back(p_observer);
    }

    auto detach(ref_observer p_observer) -> void
    {
        std::erase_if(m_observers, [&p_observer](const ref_observer &p_ref) -> bool {
            return &p_ref.get() == &p_observer.get();
        });
    }

    auto notify(const Payload &p_payload) -> void
    {
        for (const auto ob : m_observers) {
            ob.get().update(*this, p_payload);
        }
    }
};

template <typename Payload>
observer<Payload>::observer(subject<Payload> &p_subject)
    : m_subject{p_subject}
{
    m_subject.attach(*this);
}

template <typename Payload>
observer<Payload>::~observer()
{
    m_subject.detach(*this);
}

struct shift_entry
{
    size_t line_index;
    size_t word_index;
};

subject<std::string_view> g_sub_new_input;
subject<std::vector<shift_entry>> g_sub_new_line;

class lines_tokenized
{
    std::vector<std::vector<std::string_view>> m_words;

public:
    auto insert(std::vector<std::string_view> p_line) -> void {
        m_words.push_back(p_line);
        const auto line_index = m_words.size() - 1;

        std::vector<shift_entry> new_entries;
        for (size_t word_index = 0U; word_index < m_words.at(line_index).size(); ++word_index) {
            new_entries.emplace_back(line_index, word_index);
        }

        g_sub_new_line.notify(new_entries);
    }

    [[nodiscard]] auto getwords(size_t p_line_index) const -> const std::vector<std::string_view> &
    {
        return m_words.at(p_line_index);
    }
};

lines_tokenized g_lines_tokenized;

class circular_shift : public observer<std::string_view>
{
public:
    explicit circular_shift(subject_type &p_subject)
        : observer{p_subject}
    {}
    ~circular_shift() override = default;

    auto update(subject_type &, const payload_type &p_line) -> void override
    {
        g_lines_tokenized.insert(details::tokenize_line(p_line));
    }
};

class alphabetizer : public observer<std::vector<shift_entry>>
{
    std::vector<shift_entry> m_alpha_index;
public:
    explicit alphabetizer(subject_type &p_subject)
        : observer{p_subject}
    {}
    ~alphabetizer() override = default;

    auto update(subject_type &, const payload_type &p_new_entries) -> void override
    {
        const auto less = [](const shift_entry &p_lhs, const shift_entry &p_rhs) -> bool {
            const auto &lwords = g_lines_tokenized.getwords(p_lhs.line_index);
            const auto &rwords = g_lines_tokenized.getwords(p_rhs.line_index);
            const auto n = std::min(lwords.size(), rwords.size());
            for (size_t i = 0; i < n; ++i) {
                const auto &lword = lwords.at((p_lhs.word_index + i) % lwords.size());
                const auto &rword = rwords.at((p_rhs.word_index + i) % rwords.size());
                if (lword != rword) return lword < rword;
            }
            return lwords.size() < rwords.size();
        };

        for (const auto &entry : p_new_entries) {
            const auto pos = std::ranges::upper_bound(m_alpha_index, entry, less);
            m_alpha_index.insert(pos, entry);
        }
    }

    [[nodiscard]] auto getalphaindex() const -> const std::vector<shift_entry> &
    {
        return m_alpha_index;
    }
};

auto input(const std::string_view p_text) -> void
{
    size_t line_start = 0U;
    for (size_t i = 0U; i <= p_text.length(); ++i) {
        if (i == p_text.length() || p_text.at(i) == '\n') {
            const auto len = i - line_start;
            const auto line = std::string_view(&p_text.at(line_start), len);
            g_sub_new_input.notify(line);
            line_start = i + 1;
        }
    }
}

auto output(const alphabetizer &p_alpha_shift, const lines_tokenized &p_lines, size_t p_width)-> void
{
    for (const auto &[line_index, word_index] : p_alpha_shift.getalphaindex()) {
        const auto &words = p_lines.getwords(line_index);
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

auto implicit_invocation::kwic(const std::string_view p_text) -> void
{
    circular_shift shifter{g_sub_new_input};
    alphabetizer alpha{g_sub_new_line};

    input(p_text);
    output(alpha, g_lines_tokenized, 40);
}
