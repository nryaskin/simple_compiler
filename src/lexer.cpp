#include "lexer.hpp"
#include "logger.hpp"

extern logger::SimpleLogger g_logger;

namespace lexer {

std::vector<std::unique_ptr<Token>> tokenize(CompileUnit& cu) {
    g_logger.log(logger::Info, "tokenize >>");
    int line_num = 0;
    std::vector<std::unique_ptr<Token>> tokens;

    for (std::string_view line: cu) {
        int col = 0;
        while(line.length() > 0) {
            int offset = 0;
            /**
              *  Open brace {
              *  Close brace }
              *  Open parenthesis \(
              *  Close parenthesis \)
              *  Semicolon ;
              *  Int keyword int
              *  Return keyword return
              *  Identifier [a-zA-Z]\w*
              *  Integer literal [0-9]+
              */
            // NOTE: Let's remove all whitespaces preceding next token
            std::cout << "Line before offsets: " << line << std::endl;
            while(line[offset] == ' ' && offset < line.length()) {
                offset++;
            }
            line.remove_prefix(offset);
            col += offset;

            std::unique_ptr<Token> token;

            std::cout << "Line before ifs: " << line << std::endl;
            if (BraceToken::is_token(line)) {
                token = std::make_unique<BraceToken>(line_num, col);
            }
            else
            if (CloseBraceToken::is_token(line)) {
                token = std::make_unique<CloseBraceToken>(line_num, col);
            }
            else
            if (OpenParenthesisToken::is_token(line)) {
                token = std::make_unique<OpenParenthesisToken>(line_num, col);
            }
            else 
            if (CloseParenthesisToken::is_token(line)) {
                token = std::make_unique<CloseBraceToken>(line_num, col);
            }
            else 
            if (SemicolonToken::is_token(line)) {
                token = std::make_unique<SemicolonToken>(line_num, col);
            }
            else
            if (TypeKeywordToken::is_token(line)) {
                token = std::make_unique<TypeKeywordToken>(line_num, col);
            }
            else
            if (ReturnKeywordToken::is_token(line)) {
                token = std::make_unique<ReturnKeywordToken>(line_num, col);
            }
            else
            if (IdentifierToken::is_token(line)) {
                token = std::make_unique<IdentifierToken>(line_num, col);
            }
            else
            if (IntegerToken::is_token(line)) {
                token = std::make_unique<IntegerToken>(line_num, col);
            }
            else {
                throw std::runtime_error("No token matched");
            }


            auto size = token->handle_line(line);
            line.remove_prefix(size);
            col += size;
            tokens.push_back(std::move(token));
        }
        line_num++;
    }

    g_logger.log(logger::Info, "<< tokenize"); 
    return {};
}
}
