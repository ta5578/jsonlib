#pragma once
#include <string>
#include <stdexcept>
#include <map>
#include <memory>
#include <vector>

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

    class Array : public Value {
        std::vector<std::unique_ptr<Value>> _values;
    public:
        void addValue(std::unique_ptr<Value> value);
        size_t size() const;
    };

    namespace detail {

        enum class TokenType {
            LBRACE,
            RBRACE,
            COLON,
            STRING,
            COMMA,
            LBRACKET,
            RBRACKET,
            NONE
        };

        struct Token {
            TokenType type;
            std::string value;
            int line;
            int pos;

            Token(TokenType type = TokenType::NONE, const std::string& value = "", int line = 1, int pos = 1);
        };

        class Lexer {
            size_t _cursor;
            std::string _text;
            int _line;
            int _pos;

            bool isDoneReading() const;
            Token lexString();

        public:
            Lexer(const std::string& text);

            Token getToken();
        };

        class Parser {
            Lexer lexer;
            Token currentToken;

            std::unique_ptr<Object> parseObject();
            std::unique_ptr<Value> parseValue();
            std::unique_ptr<Array> parseArray();
            std::unique_ptr<Object> parseValueList();
            void raiseError(const std::string& expected);

        public:
            std::unique_ptr<Object> parse();
            Parser(Lexer lexer);
        };
    }

    std::unique_ptr<Object> parse(const std::string& text);

}