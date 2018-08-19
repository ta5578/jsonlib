#include "jsonlib.h"
#include <cctype>

namespace json {
    
    parse_exception::parse_exception(const std::string& msg)
        : std::runtime_error(msg) {}

    namespace detail {
        
        Lexer::Lexer(const std::string& text)
            : _cursor(0), _text(text) {}

        bool Lexer::isDoneReading() const
        {
            return _cursor >= _text.size();
        }

        std::string Lexer::getToken() 
        {
            // skip white space
            while (std::isspace(_text[_cursor]) && !isDoneReading()) {
                ++_cursor;
            }

            while (!isDoneReading()) {
                char c = _text[_cursor++];
                if (c == '{') {
                    return "{";
                } else if (c == '}') {
                    return "}";
                } else {
                    throw parse_exception(c + " is not a valid token!");
                }
            }

            // no more input
            return "";
        }
    }

    static std::unique_ptr<json::Object> parseObject(detail::Lexer &lexer)
    {
        auto nextTok = lexer.getToken();
        if (nextTok == "}") {
            return std::make_unique<Object>();
        } else {
            throw parse_exception("Expecting '}' token but got " + nextTok + "!");
        }
    }

    std::unique_ptr<Object> parse(const std::string& text)
    {
        detail::Lexer lexer(text);
        auto tok = lexer.getToken();
        if (tok == "{") {
            return parseObject(lexer);
        } else {
            throw parse_exception("Parser expected a '{' object token!");
        }
    }
}
