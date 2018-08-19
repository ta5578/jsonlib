#pragma once
#include <string>
#include <stdexcept>
#include <map>
#include <memory>

namespace json {

    struct parse_exception : public std::runtime_error {
        parse_exception(const std::string& msg);
    };

    class Value {};

    class Object : public Value {
        std::map<std::string, std::unique_ptr<Value>> _values;
    public:
        Value* getValue(const std::string& name);
        void addValue(const std::string& name, std::unique_ptr<Value> value);
    };

    class String : public Value {
        std::string _value;
    public:
        String(const std::string& value);
        std::string getValue() const;
    };

    namespace detail {

        enum class TokenType {
            LBRACE,
            RBRACE,
            COLON,
            STRING,
            COMMA,
            NONE
        };

        typedef std::pair<TokenType, std::string> Token;

        class Lexer {
            size_t _cursor;
            std::string _text;

            bool isDoneReading() const;
            Token lexString();

        public:
            Lexer(const std::string& text);

            Token getToken();
        };
    }

    std::unique_ptr<Object> parse(const std::string& text);

}