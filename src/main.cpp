#include <vector>
#include <string_view>
#include <set>

#include "lexer.hpp"
#include "compile_unit.hpp"
#include "logger.hpp"

logger::SimpleLogger g_logger;

std::regex lexer::IdentifierToken::reg("[a-zA-z]\\w*");
std::regex lexer::IntegerToken::reg("[0-9]+");

namespace program_options {
    // Usage:
    // compiler [-c <source_file>]
    // Options:
    // -c <source_file>
    constexpr int max_opt_cnt = 3; 

    enum parser_state_e {
        NONE,
        SOURCE_PARSING,
    };

    std::vector<std::string_view> parse(int argc, char *argv[]) {
        parser_state_e state = parser_state_e::NONE;
        std::vector<std::string_view> input_files;
        if (argc > max_opt_cnt) {
            throw std::runtime_error("Too many arguments");
        }
        const std::vector<std::string_view> args(argv + 1, argv + argc);

        for (const auto& arg : args) {
            if (state != parser_state_e::SOURCE_PARSING) {
                if (arg == "-c") {
                    state = parser_state_e::SOURCE_PARSING;
                }
                else {
                    throw std::runtime_error("Unsupported option: " + std::string(arg));
                }
            }
            else {
                if (!std::filesystem::exists(arg)) {
                    throw std::runtime_error(std::string(arg) + ": No such file");
                }
                input_files.push_back(arg);
            }
        }
        return input_files;
    }
}

namespace sl = lexer;

int main(int argc, char *argv[]) {
    auto file_names = program_options::parse(argc, argv);
    /* Defining available terminal tokens */
    
    /* Loading compilation units */
    std::vector<sl::CompileUnit> cus;
    for (auto& file_name: file_names) {
        cus.push_back(sl::CompileUnit(file_name));
    }
    for (auto& cu: cus) {
        for (auto &line: cu) {
            s_logger.log(logger::log_level_t::Info, line); 
        }
    }

    // Lexer: tokenize(cu, available_tokens) -> std::vector<std::unique_ptr<cfg::Token>>
    for (auto& cu: cus) {
        auto tokenized_cu = sl::tokenize(cu);
    }

    return 0;
}
