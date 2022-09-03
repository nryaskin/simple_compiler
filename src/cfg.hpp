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
#include <set>
#include "file_utils.hpp"
#include "tokens.hpp"


namespace sc::cfg {

    namespace str {
        template <std::string_view const &, typename, std::string_view const &, typename>
        struct concat;
        template <std::string_view const &S1,
                  std::size_t... I1,
                  std::string_view const &S2,
                  std::size_t... I2>
        struct concat<S1, std::index_sequence<I1...>, S2, std::index_sequence<I2...>> {
            static constexpr char value[] {S1[I1]...,S2[I2]..., 0};
        };

        template<std::string_view const&...>
        struct join;

        template<>
        struct join<> {
            static constexpr std::string_view value = "";
        };

        template<std::string_view const &S1, std::string_view const &S2>
        struct join<S1, S2> {
            static constexpr std::string_view value = concat<S1, std::make_index_sequence<S1.size()>,
                                                          S2, std::make_index_sequence<S2.size()>>::value;
        };

        template<std::string_view const &S, std::string_view const &...V>
        struct join<S, V...> {
            static constexpr std::string_view value = join<S, join<V...>::value>::value;
        };

        template<std::string_view const &...V>
        static constexpr auto join_v = join<V...>::value;

        static constexpr std::string_view ob = "(";
        static constexpr std::string_view cb = ")";
        template<std::string_view const &S1>
        struct grouping {
            static constexpr std::string_view value = join<ob, S1 , cb>::value;
        };

        template<std::string_view const &S1, std::string_view const &q>
        struct quantifired {
            static constexpr std::string_view value = join<grouping<S1>::value, q>::value;
        };


        static constexpr std::string_view or_sep= "|";
        template <std::string_view const&...>
        struct or_selection;

        template <std::string_view const &S1, std::string_view const &S2>
        struct or_selection<S1, S2> {
            static constexpr std::string_view value = join<S1,
                                                           or_sep,
                                                           S2>::value;
        };

        template <std::string_view const &...V>
        static constexpr auto or_selection_v = or_selection<V...>::value;

        template<std::string_view const &A1, std::string_view const &...V>
        struct or_selection<A1, V...> {
            static constexpr std::string_view value = or_selection<A1, or_selection<V...>::value>::value;
        };
    };

    class Grammar {
    public:
        Grammar(std::string_view& fpath) {
            auto file = sc::files::SFile(fpath);
            for (auto &line : file) {
            }
        }
    private:
        std::set<std::string_view> terminals;
        constexpr static std::string_view m_non_term = "<([a-zA-Z]*)>";
        constexpr static std::string_view m_term = "\"[^ ]+\"";
        constexpr static std::string_view m_regterm = "r\".+\"";
        constexpr static std::string_view m_whitspace = "[ ]*";
        constexpr static std::string_view m_eq = "=";
        constexpr static std::string_view plus = "+";

        // sreg_non_term,"[ ]*", "=", "([ ]*(", sreg_non_term, "|", sreg_term, "))+"
    public:
        constexpr static std::string_view sreg_line = str::join<m_non_term, m_whitspace, m_eq, str::quantifired<str::or_selection<m_non_term, m_term, m_regterm>::value, plus>::value>::value;
    };
};

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
