#pragma once

#include <string_view>

namespace shared_data
{
    auto kwic(std::string_view p_text) -> void;
} // namespace shared_data

namespace abstract_data
{
    auto kwic(std::string_view p_text) -> void;
} // namespace abstract_data

namespace implicit_invocation
{
    auto kwic(std::string_view p_text) -> void;
} // namespace implicit_invocation
