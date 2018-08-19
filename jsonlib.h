#pragma once
#include <string>
#include <memory>
#include <stdexcept>

namespace json {

    struct parse_exception : public std::runtime_error {
        parse_exception(const std::string& msg);
    };

    class Object {};

    namespace detail {
        class Lexer {
            size_t _cursor;
            std::string _text;

            bool isDoneReading() const;
        public:
            Lexer(const std::string& text);

            std::string getToken();
        };
    }

    std::unique_ptr<Object> parse(const std::string& text);

}