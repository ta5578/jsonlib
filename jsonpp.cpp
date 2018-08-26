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

        void Lexer::raiseError(const std::string& expected)
        {
            throw parse_exception(json::detail::format("Expecting '%s' at (%d:%d) but found '%c' instead!", expected.c_str(), _line, _pos, curr()));
        }

        Lexer::NumberState Lexer::processState(NumberState state, std::string& value)
        {
            char c = curr();
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
                    raiseError("<digit> or <decimal>");
                }
                break;
            case DECIMAL:
                if (std::isdigit(c)) {
                    value += c;
                } else if (c == 'e' || c == 'E') {
                    value += c;
                    state = EXPONENT;
                } else {
                    raiseError("<digit> or <exponent>");
                }
                break;
            case EXPONENT:
                if (c == '+' || c == '-') {
                    value += c;
                } else if (std::isdigit(c)) {
                    value += c;
                    state = EXPONENT_DIGIT;
                } else {
                    raiseError("<digit> or <sign>");
                }
                break;
            case EXPONENT_DIGIT:
                if (std::isdigit(c)) {
                    value += c;
                } else {
                    raiseError("<digit> after exponent value");
                }
                break;
            default:
                break;
            }

            if (state != END) {
                next(); // get the next character ready
            }

            return state;
        }

        Token Lexer::lexNumber()
        {
            const char initialChar = curr();
            const int initialPosition = _pos; // get the position for reporting

            NumberState state;
            if (initialChar == '+' || initialChar == '-') {
                state = SIGN;
            } else {
                state = DIGIT;
            }
            next(); // eat the first char

            std::string value(1, initialChar);
            while (!isDoneReading() && state != END) {
                state = processState(state, value);
            }
            return { TokenType::NUMBER, value, _line, initialPosition };
        }

        std::string Lexer::lexValueSequence(const std::string& expected)
        {
            std::string value;
            char c = curr();
            for (size_t i = 0; i < expected.length() && !isDoneReading(); ++i) {
                if (c != expected[i]) {
                    raiseError("value sequence " + expected);
                }
                value += c;
                c = next();
            }
            return value;
        }

        Token Lexer::lexBool(const std::string& expected)
        {
            const int initialPosition = _pos; // get the position for reporting
            auto value = lexValueSequence(expected);
            return { TokenType::JBOOL, value, _line, initialPosition };
        }

        Token Lexer::lexNull()
        {
            const int initialPosition = _pos; // get the position for reporting
            auto value = lexValueSequence("null");
            return { TokenType::JNULL, value, _line, initialPosition };
        }

        bool Lexer::isWhitespaceControlChar(char n) const
        {
            return n == 'b' || n == 'f' || n == 'n' || n == 'r' || n == 't';
        }

        bool Lexer::isControlChar(char c) const
        {
            return c == '\"' || c == '\\' || c == '/' || isWhitespaceControlChar(c) || c == 'u';
        }

        char Lexer::getControlChar()
        {
            char n = peek();
            if (n == EOF) {
                throw parse_exception(json::detail::format("Dangling control '\\' found at (%d:%d)!", _line, _pos));
            }
            next(); // eat the control character

            if (isControlChar(n)) {
                return n;
            } else {
                raiseError(R"(("|\|/|b|f|n|r|t) control character)");
            }
            return EOF;
        }

        bool Lexer::isHexChar(char c)
        {
            const auto lower = std::tolower(c);
            return std::isdigit(c) || (lower >= 'a' && lower <= 'f');
        }

        std::string Lexer::getHexDigits()
        {
            std::string digits;
            for (char c = curr(); !isDoneReading(); c = next()) {
                if (isHexChar(c)) {
                    digits += c;
                } else {
                    --_cursor; // leave the lexer at the last valid character
                    break;
                }
            }
            
            if (digits.length() != 4) {
                throw parse_exception(json::detail::format("Only 4 hexadecimal values accepted at (%d:%d)", _line, _pos));
            }

            return digits;
        }

        Token Lexer::lexString()
        {
            if (curr() != '\"') {
                raiseError("initial \" for string");
            }
            const int initialPosition = _pos; // get the position for reporting
            next(); // eat the quote

            std::string str;
            bool endQuoteFound = false;
            for (char c = curr(); !endQuoteFound && !isDoneReading(); c = next()) {
                if (c == '\"') {
                    endQuoteFound = true;
                } else if (c == '\\') {
                    char n = getControlChar();
                    if (isWhitespaceControlChar(n)) {
                        str += c;
                        str += n;
                    } else if (n == 'u') {
                        next(); // eat the 'u'
                        str += getHexDigits();
                    } else {
                        str += n;
                    }
                } else {
                    str += c;
                }
            }

            if (!endQuoteFound) {
                raiseError("Terminating \" for string");
            }
            return {TokenType::STRING, str, _line, initialPosition };
        }

        char Lexer::next()
        {
            ++_pos;
            return _text[++_cursor];
        }

        char Lexer::curr()
        {
            return _text[_cursor];
        }

        char Lexer::peek()
        {
            if (_cursor + 1 >= _text.size()) {
                return EOF;
            }
            return _text[_cursor + 1];
        }

        void Lexer::skipWhitespace()
        {
            for (char c = curr(); !isDoneReading() && std::isspace(c); c = next()) {
                if (c == '\n') {
                    ++_line;
                    _pos = 0;
                }
            }
        }

        Token Lexer::reportToken(TokenType type, const std::string& str)
        {
            const int currentPosition = _pos;
            next(); // eat the token
            return { type, str, _line, currentPosition };
        }

        Token Lexer::getToken() 
        {
            skipWhitespace();
            if (isDoneReading()) {
                return { TokenType::NONE, "" };
            }

            char c = curr();
            if (c == '{') {
                return reportToken(TokenType::LBRACE, "{");
            } else if (c == '}') {
                return reportToken(TokenType::RBRACE, "}");
            } else if (c == '\"') {
                return lexString();
            } else if (c == ':') {
                return reportToken(TokenType::COLON, ":");
            } else if (c == ',') {
                return reportToken(TokenType::COMMA, ",");
            } else if (c == '[') {
                return reportToken(TokenType::LBRACKET, "[");
            } else if (c == ']') {
                return reportToken(TokenType::RBRACKET, "{");
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
