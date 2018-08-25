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

    class Value {
    protected:
        enum class ValueType {
            OBJECT,
            ARRAY,
            STRING,
            BOOL,
            JNULL,
            NUMBER
        } _type;

        bool isType(ValueType type) const;
        Value(ValueType type);

    public:
        bool isObject() const;
        bool isArray() const;
        bool isString() const;
        bool isNumber() const;
        bool isNull() const;
        bool isBool() const;
    };

    class Object;
    class Array;

    class Array : public Value {
        std::vector<std::unique_ptr<Value>> _values;
    public:
        Array();
        void addValue(std::unique_ptr<Value> value);
        size_t size() const;

        Value* getValue(size_t index) const;
        Object* getObjectValue(size_t index) const;
        Array* getArrayValue(size_t index) const;

        std::string getStringValue(size_t index, const std::string& defaulValue = "") const;
        bool getBoolValue(size_t index, bool defaultValue = false) const;
        double getNumberValue(size_t index, double defaultValue = 0.0f) const;
        void* getNullValue(size_t index) const;

    };

    class Object : public Value {
        std::map<std::string, std::unique_ptr<Value>> _values;
    public:
        Object();

        Value* getValue(const std::string& name) const;
        Object* getObjectValue(const std::string& name) const;
        Array* getArrayValue(const std::string& name) const;

        std::string getStringValue(const std::string& name, const std::string& defaulValue = "") const;
        bool getBoolValue(const std::string& name, bool defaultValue = false) const;
        double getNumberValue(const std::string& name, double defaultValue = 0.0f) const;
        void* getNullValue(const std::string& name) const;

        void addValue(const std::string& name, std::unique_ptr<Value> value);
    };

    class String : public Value {
        std::string _value;
    public:
        String(const std::string& value);
        std::string getValue() const;
    };

    class Bool : public Value {
        bool _value;
    public:
        Bool(bool value);
        bool getValue() const;
    };

    class Null : public Value {
    public:
        Null();
        void* getValue() const;
    };

    class Number : public Value {
        double _value;
    public:
        Number(double value);
        double getValue() const;
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
            JBOOL, // this name prevents macro collision with BOOL macro from WinAPI
            JNULL, // this name prevents macro collision with NULL macro
            NUMBER,
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

            enum NumberState { SIGN, DIGIT, DECIMAL, EXPONENT, EXPONENT_DIGIT, END };

            Lexer::NumberState Lexer::processState(NumberState state, std::string& value);

            bool isDoneReading() const;
            
            bool isWhitespaceControlChar(char n) const;
            bool isControlChar(char c) const;
            char getControlChar();
            Token lexString();

            std::string lexValueSequence(const std::string& expected);
            Token lexBool(const std::string& expected);
            Token lexNull();

            Token lexNumber();

            void raiseError(const std::string& expected);

            void skipWhitespace();
            char next();
            char curr();
            char peek();

            Token reportToken(TokenType type, const std::string& str);

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