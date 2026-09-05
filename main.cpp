#include <string_view>
#include "kwic.h"
#include <fmt/format.h>
#include <CLI/CLI.hpp>
#include <fstream>

auto main(int argc, char *argv[]) -> int
{
    CLI::App app{"KWIC architectural style demo"};

    std::string method;
    app.add_option("--method", method, "Architectural style to run")
       ->required()
       ->check(CLI::IsMember({"shared_data", "abstract_data", "implicit_invocation", "filters"}));

    size_t width = 40;
    app.add_option("--width", width, "Output column width")
       ->default_val(width);

    std::string file_path;
    app.add_option("--file", file_path, "Path to input text file")
        ->required();

    CLI11_PARSE(app, argc, argv);

    if (method == "shared_data") {
        shared_data::kwic(file_path, width);
    } else if (method == "abstract_data") {
        abstract_data::kwic(file_path, width);
    } else if (method == "implicit_invocation") {
        implicit_invocation::kwic(file_path, width);
    } else {
        filters::kwic(file_path, width);
    }
}
