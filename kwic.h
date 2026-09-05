#pragma once

#include <string_view>

namespace shared_data
{
    auto kwic(const std::string &p_path, size_t p_width) -> void;
} // namespace shared_data

namespace abstract_data
{
    auto kwic(const std::string &p_path, size_t p_width) -> void;
} // namespace abstract_data

namespace implicit_invocation
{
    auto kwic(const std::string &p_path, size_t p_width) -> void;
} // namespace implicit_invocation

namespace filters
{
    auto kwic(const std::string &p_path, size_t p_width) -> void;
} // namespace filters
