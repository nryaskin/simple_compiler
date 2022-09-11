#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <string_view>
#include <array>
#include <memory>
#include <type_traits>

namespace sc::tokens {
    using namespace std::literals;
    class Token {
    };

    template <typename T1, typename T2>
    struct TokenLink {
        std::unique_ptr<T2> m_next_token;
        T1 m_token;
    };

//    template <std::string_view const&, typename>
//    class KeywordToken;
//    // Note: We can use template here and for each string_view there will be different type of KeywordToken.
//    template <std::string_view const& S,
//              std::size_t ...Idx>
//    class KeywordToken<S, std::index_sequence<Idx...>> : public Token {
//    public:
//        KeywordToken(std::string_view& s) : Token() {
//
//        }
//    private:
//        static constexpr std::string_view keyword = {S[Idx]...};
//    };
//
//    template<std::string_view const&S>
//    class KeywordToken<S, std::make_index_sequence<S.size()>>;

    template<char ...>
    class KeywordTokenImpl;

    template <std::string_view const&S, typename>
    struct KW;

    template <std::string_view const&S, std::size_t ...Idx>
    struct KW<S, std::index_sequence<Idx...>> {
        using type = KeywordTokenImpl<S[Idx]..., 0>; 
    };

    // KeywordToken<'r', 'e', 't', 'u', 'r', 'n'>
    template<char ...Chs>
    class KeywordTokenImpl : public Token {

    };

    template<std::string_view const&S>
    struct KeywordToken {
        using token_type = typename KW<S, std::make_index_sequence<S.size()>>::type;
    };

#if 0
    // Note: We can use template and for each RegexToken there will be different type for compiler
    class RegexToken : public Token {
    public:
        RegexToken(std::string_view& s) : Token() {

        }
    };

    RegexToken operator""_reg(const char *s, size_t n) {
        auto a = std::string_view(s, n);
        return RegexToken(a);
    }
#endif /* RegexToken */

    // Note: We can pass some kind of string initializer { KeywordToken<"return">, RegexToken<"[0-9]+">, KeywordToken<";"> }
    // I shall generate token valiadtions depending on this type initializer list.
    // I guess each Token Type shall have distinct is_token function which will allow to write something like (T::is_token(line); move_start(line); ...T)
    template<typename ...Ts>
    class ComplexToken;

    template<std::size_t, typename, typename, typename ...>
    class ComplexTokenPrepend;

    template<typename T, typename T0, typename ...Ts>
    class ComplexTokenPrepend<0, T, T0, Ts...> {
        using type = ComplexToken<T0, Ts...>;
    };

    template<std::size_t N, typename T, typename T0, typename ...Ts>
    class ComplexTokenPrepend {
        using type = typename ComplexTokenPrepend<N - 1, T, T0, typename std::tuple_element<N - 1, T>::type, Ts...>::type;
    };

    template<std::size_t, typename, typename, typename ...>
    class ComplexTokenAppend;

    template<typename T, typename T0, typename ...Ts>
    class ComplexTokenAppend<0, T, T0, Ts...> {
        using type = ComplexToken<Ts..., T0>;
    };

    template<std::size_t N, typename T, typename T0, typename ...Ts>
    class ComplexTokenAppend {
        using type = typename ComplexTokenAppend<N - 1, T, T0, typename std::tuple_element<N - 1, T>::type, Ts...>::type;
    };

    template<typename ...Ts>
    class ComplexToken {
        using type = typename std::tuple<Ts...>;
    };


    // KeywordToken x KeywordToken -> ComplexToken
    template<std::string_view const&S1, std::string_view const&S2>
    ComplexToken<KeywordToken<S1>, KeywordToken<S2>> operator<<(KeywordToken<S1> kt1, KeywordToken<S2> kt2) {
        return ComplexToken<KeywordToken<S1>, KeywordToken<S2>>();
    }

    template<typename T1, typename ...Ts2>
    ComplexToken<T1, Ts2...> operator<<(T1 t1, ComplexToken<Ts2...> ct2) {
        using ct2_tuple_type = typename ComplexToken<Ts2...>::type;
        using ret_type = typename ComplexTokenPrepend<std::tuple_size_v<ct2_tuple_type>, ct2_tuple_type, T1>::type; 
        return ret_type();
    }

    template<typename ...Ts1, typename T2>
    ComplexToken<Ts1..., T2> operator<<(ComplexToken<Ts1...> ct1, T2 t2) {
        using ct1_tuple_type = typename ComplexToken<Ts1...>::type;
        using ret_type = typename ComplexTokenAppend<std::tuple_size_v<ct1_tuple_type>, ct1_tuple_type, T2>::type; 
        return ret_type();
    }

    template<typename ...Ts>
    struct TokenRule {
        using type = std::tuple<Ts...>;

        TokenRule<Ts...> operator=(ComplexToken<Ts...> ct) {
            return TokenRule<Ts...>();
        };
    };

};

#endif /* _TOKENS_H_ */
