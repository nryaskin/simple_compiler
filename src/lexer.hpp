#ifndef _LEXER_H_
#define _LEXER_H_

/**
 * Author: nikita.ryaskin
 * Description: Tokenizer related functions.
 */
#include <set>

#include "cfg.hpp"
#include "file_utils.hpp"

namespace lexer {
    std::vector<std::unique_ptr<Token>> tokenize(sc::files::SFile& cu);
}


#endif /* _LEXER_H_ */
