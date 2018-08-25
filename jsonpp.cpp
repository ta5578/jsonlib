#include "jsonpp.hpp"
#include <cctype>
#include <cstdlib>

namespace json {
    
    parse_exception::parse_exception(const std::string& msg)
        : std::runtime_error(msg) {}

    bool Value::isType(ValueType type) const
    {
        return _type == type;
    }

    Value::Value(ValueType type)
        : _type(type) {}

    bool Value::isObject() const
    {
        return isType(ValueType::OBJECT);
    }

    bool Value::isArray() const
    {
        return isType(ValueType::ARRAY);
    }

    bool Value::isString() const
    {
        return isType(ValueType::STRING);
    }

    bool Value::isNumber() const
    {
        return isType(ValueType::NUMBER);
    }

    bool Value::isNull() const
    {
        return isType(ValueType::JNULL);
    }

    bool Value::isBool() const
    {
        return isType(ValueType::BOOL);
    }

    Object::Object()
        : Value(Value::ValueType::OBJECT) {}

    Value* Object::getValue(const std::string& name) const
    {
        auto it = _values.find(name);
        if (it != _values.end()) {
            auto& value = it->second;
            return value.get();
        }

        // recursively search for the value in child values that are objects
        for (auto& p : _values) {
            auto& value = p.second;
            if (value->isObject()) {
                return static_cast<Object*>(value.get())->getValue(name);
            }
        }
        return nullptr;
    }

    void Object::addValue(const std::string& name, std::unique_ptr<Value> value)
    {
        _values.emplace(name, std::move(value));
    }

    Object* Object::getObjectValue(const std::string& name) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isObject()) {
            return nullptr;
        }
        return static_cast<Object*>(value);
    }

    Array* Object::getArrayValue(const std::string& name) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isArray()) {
            return nullptr;
        }
        return static_cast<Array*>(value);
    }

    std::string Object::getStringValue(const std::string& name, const std::string& defaultValue) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isString()) {
            return defaultValue;
        }
        return static_cast<String*>(value)->getValue();
    }

    bool Object::getBoolValue(const std::string& name, bool defaultValue) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isBool()) {
            return defaultValue;
        }
        return static_cast<Bool*>(value)->getValue();
    }

    void* Object::getNullValue(const std::string& name) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isNull()) {
            return nullptr;
        }
        return nullptr;
    }

    double Object::getNumberValue(const std::string& name, double defaultValue) const
    {
        auto value = getValue(name);
        if (value == nullptr || !value->isNumber()) {
            return defaultValue;
        }
        return static_cast<Number*>(value)->getValue();
    }

    String::String(const std::string& value)
        : Value(Value::ValueType::STRING), _value(value) {}

    std::string String::getValue() const
    {
        return _value;
    }

    Array::Array()
        : Value(Value::ValueType::ARRAY) {}

    void Array::addValue(std::unique_ptr<Value> value)
    {
        _values.emplace_back(std::move(value));
    }

    size_t Array::size() const
    {
        return _values.size();
    }

    Value* Array::getValue(size_t index) const
    {
        return _values.at(index).get();
    }

    Object* Array::getObjectValue(size_t index) const
    {
        auto value = getValue(index);
        if (value == nullptr || !value->isObject()) {
            return nullptr;
        }
        return static_cast<Object*>(value);
    }

    Array* Array::getArrayValue(size_t index) const
    {
        auto value = getValue(index);
        if (value == nullptr || !value->isArray()) {
            return nullptr;
        }
        return static_cast<Array*>(value);
    }

    std::string Array::getStringValue(size_t index, const std::string& defaultValue) const
    {
        auto value = getValue(index);
        if (value == nullptr || !value->isString()) {
            return defaultValue;
        }
        return static_cast<String*>(value)->getValue();
    }

    bool Array::getBoolValue(size_t index, bool defaultValue) const
    {
        auto value = getValue(index);
        if (value == nullptr || !value->isBool()) {
            return defaultValue;
        }
        return static_cast<Bool*>(value)->getValue();
    }

    double Array::getNumberValue(size_t index, double defaultValue) const
    { 
        auto value = getValue(index);
        if (value == nullptr || !value->isNumber()) {
            return defaultValue;
        }
        return static_cast<Number*>(value)->getValue();
    }

    void* Array::getNullValue(size_t index) const
    {
        auto value = getValue(index);
        if (value == nullptr || !value->isNull()) {
            return nullptr;
        }
        return nullptr;
    }

    std::unique_ptr<Object> parse(const std::string& text)
    {
        detail::Lexer lexer(text);
        detail::Parser parser(lexer);
        return parser.parse();
    }

    Bool::Bool(bool value)
        : Value(Value::ValueType::BOOL), _value(value) {}

    bool Bool::getValue() const
    {
        return _value;
    }

    Null::Null()
        : Value(Value::ValueType::JNULL) {}

    void* Null::getValue() const
    {
        return nullptr;
    }

    Number::Number(double value)
        : Value(Value::ValueType::NUMBER), _value(value) {}

    double Number::getValue() const
    {
        return _value;
    }

    namespace detail {

        template <class... Args>
        static std::string format(const char* fmt, Args&&... args)
        {
            auto len = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...) + 1;
            std::vector<char> buf(len, '\0');
            std::snprintf(buf.data(), len, fmt, std::forward<Args>(args)...);
            return buf.data();
        }

        Token::Token(TokenType type, const std::string& value, int line, int pos)
            : type(type), value(value), line(line), pos(pos) {}
        
        Lexer::Lexer(const std::string& text)
            : _cursor(0), _text(text), _line(1), _pos(1) {}

        bool Lexer::isDoneReading() const
        {
            return _cursor >= _text.size();
        }

        Token Lexer::lexNumber()
        {
            enum NumberState { SIGN, DIGIT, DECIMAL, EXPONENT, EXPONENT_DIGIT, END} state;
            char initialChar = curr();
            if (initialChar == '+' || initialChar == '-') {
                state = SIGN;
            } else {
                state = DIGIT;
            }
            next(); // eat the first char

            std::string value(1, initialChar);
            for (char c = curr(); !isDoneReading() && state != END; c = next()) {
                if (std::isspace(c)) {
                    state = END;
                }

                switch (state) {
                case SIGN:
                    if (std::isdigit(c)) {
                        value += c;
                        state = DIGIT;
                    }
                    break;
                case DIGIT:
                    if (std::isdigit(c)) {
                        value += c;
                    } else if (c == '.') {
                        value += c;
                        state = DECIMAL;
                    } else {
                        throw parse_exception(json::detail::format("Expecting <digit> or <decimal> at (%d:%d) but found '%c' found!", _line, _pos, c));
                    }
                    break;
                case DECIMAL:
                    if (std::isdigit(c)) {
                        value += c;
                    } else if (c == 'e' || c == 'E') {
                        value += c;
                        state = EXPONENT;
                    } else {
                        throw parse_exception(json::detail::format("Expecting <digit> or <exponent> at (%d:%d) but '%c' found!", _line, _pos, c));
                    }
                    break;
                case EXPONENT:
                    if (c == '+' || c == '-') {
                        value += c;
                    } else if (std::isdigit(c)) {
                        value += c;
                        state = EXPONENT_DIGIT;
                    } else {
                        throw parse_exception(json::detail::format("Expecting <digit> or <sign> at (%d:%d) but '%c' found!", _line, _pos, c));
                    }
                    break;
                case EXPONENT_DIGIT:
                    if (std::isdigit(c)) {
                        value += c;
                    } else {
                        throw parse_exception(json::detail::format("Expecting <digit> after exponent value at (%d:%d) but '%c' found!", _line, _pos, c));
                    }
                    break;
                default:
                    break;
                }
            }
            return { TokenType::NUMBER, value, _line, _pos };
        }

        std::string Lexer::lexValueSequence(const std::string& expected)
        {
            std::string value;
            char c = curr();
            for (size_t i = 0; i < expected.length() && !isDoneReading(); ++i) {
                if (c != expected[i]) {
                    throw parse_exception(json::detail::format("Expecting value sequence '%s' but found '%c' instead!", expected.c_str(), c));
                }
                value += c;
                c = next();
            }
            return value;
        }

        Token Lexer::lexBool(const std::string& expected)
        {
            auto value = lexValueSequence(expected);
            return { TokenType::JBOOL, value, _line, _pos };
        }

        Token Lexer::lexNull()
        {
            auto value = lexValueSequence("null");
            return { TokenType::JNULL, value, _line, _pos };
        }

        Token Lexer::lexString()
        {
            if (curr() != '\"') {
                throw parse_exception("Expected \" string type!");
            }
            next(); // eat the quote

            std::string str;
            bool endQuoteFound = false;
            for (char c = curr(); !endQuoteFound && !isDoneReading(); c = next()) {
                if (c == '\"') {
                    endQuoteFound = true;
                } else {
                    str += c;
                }
            }

            if (!endQuoteFound) {
                throw parse_exception("Terminating \" for string not found!");
            }

            return {TokenType::STRING, str, _line, _pos };
        }

        char Lexer::next()
        {
            return _text[++_cursor];
        }

        char Lexer::curr()
        {
            return _text[_cursor];
        }

        void Lexer::skipWhitespace()
        {
            for (char c = curr(); !isDoneReading() && std::isspace(c); c = next()) {
                if (c == '\n') {
                    ++_line;
                    _pos = 1;
                } else {
                    ++_pos;
                }
            }
        }

        Token Lexer::getToken() 
        {
            skipWhitespace();
            if (isDoneReading()) {
                return { TokenType::NONE, "" };
            }

            char c = curr();
            if (c == '{') {
                next(); // eat the brace
                return { TokenType::LBRACE, "{", _line, _pos };
            } else if (c == '}') {
                next(); // eat the brace
                return { TokenType::RBRACE, "}", _line, _pos };
            } else if (c == '\"') {
                return lexString();
            } else if (c == ':') {
                next(); // eat the colon
                return { TokenType::COLON, ":", _line, _pos };
            } else if (c == ',') {
                next(); // eat the comma
                return { TokenType::COMMA, ",", _line, _pos };
            } else if (c == '[') {
                next(); // eat the bracket
                return { TokenType::LBRACKET, "[", _line, _pos };
            } else if (c == ']') {
                next(); // eat the bracket
                return { TokenType::RBRACKET, "]", _line, _pos };
            } else if (c == 't') {
                return lexBool("true");
            } else if (c == 'f') {
                return lexBool("false");
            } else if (c == 'n') {
                return lexNull();
            } else if (c == '-' || c == '+' || std::isdigit(c)) {
                return lexNumber();
            } else {
                throw parse_exception(c + " is not a valid token!");
            }
        }

        void Parser::raiseError(const std::string& expected)
        {
            throw parse_exception(json::detail::format("Expecting '%s' at line %d:%d but got '%s' instead!", expected.c_str(), currentToken.line, currentToken.pos, currentToken.value.c_str()));
        }

        std::unique_ptr<Object> Parser::parseObject()
        {
            currentToken = lexer.getToken();
            if (currentToken.type != detail::TokenType::LBRACE) {
                raiseError("{");
            }

            currentToken = lexer.getToken();

            // an empty object
            if (currentToken.type == detail::TokenType::RBRACE) {
                return std::make_unique<Object>();
            }

            auto obj = parseValueList();

            // closing brace
            if (currentToken.type != detail::TokenType::RBRACE) {
                raiseError("}");
            }

            return obj;
        }

        std::unique_ptr<Array> Parser::parseArray()
        {
            currentToken = lexer.getToken();
            // empty array
            if (currentToken.type == TokenType::RBRACKET) {
                return std::make_unique<Array>();
            }

            auto arr = std::make_unique<Array>();
            bool isList = false;
            do {
                arr->addValue(parseValue());

                currentToken = lexer.getToken();
                if (currentToken.type == TokenType::COMMA) {
                    isList = true;
                    currentToken = lexer.getToken(); // eat the comma
                } else {
                    isList = false;
                }
            } while (isList);

            if (currentToken.type != TokenType::RBRACKET) {
                raiseError("]");
            }

            return arr;
        }

        std::unique_ptr<Value> Parser::parseValue()
        {
            if (currentToken.type == detail::TokenType::STRING) {
                return std::make_unique<json::String>(currentToken.value);
            } else if (currentToken.type == detail::TokenType::LBRACKET) {
                return parseArray();
            } else if (currentToken.type == detail::TokenType::JBOOL) {
                return std::make_unique<json::Bool>(currentToken.value == "true");
            } else if (currentToken.type == detail::TokenType::JNULL) {
                return std::make_unique<json::Null>();
            } else if (currentToken.type == detail::TokenType::NUMBER) {
                return std::make_unique<json::Number>(std::stod(currentToken.value));
            } else {
                raiseError("<value>");
            }
            return nullptr;
        }

        std::unique_ptr<json::Object> Parser::parseValueList()
        {
            auto obj = std::make_unique<Object>();
            bool isList = false;
            do {
                // name
                if (currentToken.type != detail::TokenType::STRING) {
                    raiseError("<string>");
                }
                auto name = currentToken.value;

                // colon
                currentToken = lexer.getToken();
                if (currentToken.type != detail::TokenType::COLON) {
                    raiseError(":");
                }
                currentToken = lexer.getToken(); // eat the colon

                // value
                auto value = parseValue();
                obj->addValue(name, std::move(value));

                // if there is a comma, we continue parsing the list
                // otherwise, we are at the end of the name/value pairs
                currentToken = lexer.getToken();
                if (currentToken.type == detail::TokenType::COMMA) {
                    isList = true;
                    currentToken = lexer.getToken(); // eat the comma
                } else {
                    isList = false;
                }

            } while (isList);
            return obj;
        }

        std::unique_ptr<Object> Parser::parse()
        {
            return parseObject();
        }

        Parser::Parser(Lexer lexer)
            : lexer(lexer) {}
    }
}
