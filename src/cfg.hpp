#ifndef _CFG_H_
#define _CFG_H_
/**
 * Author: Nikita Ryaskin<nik.ryaskin@gmail.com>
 * Data structures to define Context Free Grammar for Parser and Lexer.
 *
 */
#include <string_view>
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <iostream>


namespace lexer {
    /**
     *
     *  Open brace {
     *  Close brace }
     *  Open parenthesis \(
     *  Close parenthesis \)
     *  Semicolon ;
     *  Int keyword int
     *  Return keyword return
     *  Identifier [a-zA-Z]\w*
     *  Integer literal [0-9]+
     *
     */
    class Token {
    public:
        Token(int line, int col) : m_line(line), m_col(col) {}
        virtual size_t handle_line(std::string_view& line) = 0;
    private:
        int m_line;
        int m_col;
    };

    template <char symbol>
    class OneSymbolToken : public Token {
    public:
        OneSymbolToken(int line_num, int col) : Token(line_num, col) {
            std::cout << "Create OneSymbolToken: " << symbol << "[ " << line_num << ":" << col  << " ]" << std::endl;
        }
        size_t handle_line(std::string_view& line) {
            return 1;
        }

        static bool is_token(std::string_view& line) {
            if (line[0] == symbol) {
                return true;
            }
            return false;
        }
    };

    class BraceToken : public OneSymbolToken<'{'> {
        public:
            BraceToken(int line_num, int col) : OneSymbolToken<'{'>(line_num, col) {}
    };

    class CloseBraceToken : public OneSymbolToken<'}'> {
        public:
            CloseBraceToken(int line_num, int col) : OneSymbolToken<'}'>(line_num, col) {}
    };

    class OpenParenthesisToken : public OneSymbolToken<'('> {
        public:
            OpenParenthesisToken(int line_num, int col) : OneSymbolToken<'('>(line_num, col) {}
    };

    class CloseParenthesisToken : public OneSymbolToken<')'> {
        public:
            CloseParenthesisToken(int line_num, int col) : OneSymbolToken<')'>(line_num, col) {}
    };

    class SemicolonToken : public OneSymbolToken<';'> {
        public:
            SemicolonToken(int line_num, int col) : OneSymbolToken<';'>(line_num, col) {}
    };

    class KeywordToken : public Token {
    public:
        KeywordToken(int line_num, int col) : Token(line_num, col) {}
        virtual std::string_view get_keyword() const = 0;
        size_t handle_line(std::string_view& line) {
            auto keyword = get_keyword();
            return keyword.length();
        }

        static bool is_token(std::string_view& line, const std::string_view& keyword) {
            if (line.compare(0, keyword.length(), keyword) == 0) {
                return true;
            }
            return false;
        }
    };

    class TypeKeywordToken : public KeywordToken {
    public:
        TypeKeywordToken(int line_num, int col) : KeywordToken(line_num, col) {
            std::cout << "Create TypeKeywordToken: " << TypeKeywordToken::kw << " [ " << line_num << ":" << col  << " ]" << std::endl;
        }
        std::string_view get_keyword() const override {
            return kw;
        }

        static bool is_token(std::string_view& line) {
            return KeywordToken::is_token(line, TypeKeywordToken::kw);
        }

    private:
        static constexpr std::string_view kw = "int";
    };

    class ReturnKeywordToken : public KeywordToken {
    public:
        ReturnKeywordToken(int line_num, int col) : KeywordToken(line_num, col) {
            std::cout << "Create ReturnKeywordToken: " << ReturnKeywordToken::kw << " [ " << line_num << ":" << col  << " ]" << std::endl;
        }
        std::string_view get_keyword() const override {
            return kw;
        }

        static bool is_token(std::string_view& line) {
            return KeywordToken::is_token(line, ReturnKeywordToken::kw);
        }

    private:
        static constexpr std::string_view kw = "return";
    };

    class VariableToken : public Token {
    public:
        VariableToken(int line_num, int col, std::regex r) : Token(line_num, col), m_reg(r) {}

        size_t handle_line(std::string_view& line) {
            std::smatch val_match;
            std::string l(line);

            if (std::regex_search(l, val_match, m_reg)) {
                m_value = val_match.str();
            }
            else {
                throw std::runtime_error("Cannot match variable token: " + l);
            }

            return m_value.length();
        }


    protected:
        std::string_view m_value;
        std::regex m_reg;
    };

    class IdentifierToken : public VariableToken {
    public:
        IdentifierToken(int line_num, int col) : VariableToken(line_num, col, IdentifierToken::reg) {
            std::cout << "Create IdentifierToken: " << " [ " << line_num << ":" << col  << " ]" << std::endl;
        }

        static bool is_token(std::string_view& line) {
            std::string l(line);
            return std::regex_search(l, IdentifierToken::reg);
        }

    private:
        static std::regex reg;
    };

    class IntegerToken : public VariableToken {
    public:
        IntegerToken(int line_num, int col) : VariableToken(line_num, col, IntegerToken::reg) {
            std::cout << "Create IntegerToken: " << " [ " << line_num << ":" << col  << " ]" << std::endl;
        }

        static bool is_token(std::string_view& line) {
            std::string l(line);
            return std::regex_search(l, IntegerToken::reg);
        }

    private:
        std::string_view  ident;
        static std::regex reg;
    };
}

#endif /* _CFG_H_ */
