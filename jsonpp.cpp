#include "jsonpp.hpp"
#include <cctype>
#include <cstdlib>

namespace json {
    
    parse_exception::parse_exception(const std::string& msg)
        : std::runtime_error(msg) {}

    Value* Object::getValue(const std::string& name)
    {
        auto it = _values.find(name);
        if (it == _values.end()) {
            return nullptr;
        }
        return _values[name].get();
    }

    void Object::addValue(const std::string& name, std::unique_ptr<Value> value)
    {
        _values.emplace(name, std::move(value));
    }

    String::String(const std::string& value)
        : _value(value) {}

    std::string String::getValue() const
    {
        return _value;
    }

    void Array::addValue(std::unique_ptr<Value> value)
    {
        _values.emplace_back(std::move(value));
    }

    size_t Array::size() const
    {
        return _values.size();
    }

    std::unique_ptr<Object> parse(const std::string& text)
    {
        detail::Lexer lexer(text);
        detail::Parser parser(lexer);
        return parser.parse();
    }

    Bool::Bool(bool value)
        : _value(value) {}

    bool Bool::getValue() const
    {
        return _value;
    }

    void* Null::getValue() const
    {
        return nullptr;
    }

    Number::Number(double value)
        : _value(value) {}

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

        Token Lexer::lexNumber(char initialChar)
        {
            enum NumberState { SIGN, DIGIT, DECIMAL, EXPONENT, EXPONENT_DIGIT, END} state;
            if (initialChar == '+' || initialChar == '-') {
                state = SIGN;
            } else {
                state = DIGIT;
            }

            std::string value(1, initialChar);
            while (!isDoneReading() && state != END) {
                char c = _text[_cursor++];
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

        std::string Lexer::lexValueSequence(char initialChar, const std::string& expected)
        {
            std::string value(1, initialChar);
            for (size_t i = 1; i < expected.length() && !isDoneReading(); ++i) {
                char c = _text[_cursor++];
                if (c != expected[i]) {
                    throw parse_exception(json::detail::format("Expecting value sequence '%s' but found '%c' instead!", expected.c_str(), c));
                }
                value += c;
            }
            return value;
        }

        Token Lexer::lexBool(char initialChar, const std::string& expected)
        {
            auto value = lexValueSequence(initialChar, expected);
            return { TokenType::JBOOL, value, _line, _pos };
        }

        Token Lexer::lexNull()
        {
            auto value = lexValueSequence('n', "null");
            return { TokenType::JNULL, value, _line, _pos };
        }

        Token Lexer::lexString()
        {
            std::string str;
            bool endQuoteFound = false;
            while (!endQuoteFound && !isDoneReading()) {
                char c = _text[_cursor++];
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


        Token Lexer::getToken() 
        {
            // skip white space and account for lines and position in the line
            while (std::isspace(_text[_cursor]) && !isDoneReading()) {
                if (_text[_cursor] == '\n') {
                    ++_line;
                    _pos = 1;
                } else {
                    ++_pos;
                }
                ++_cursor;
            }

            while (!isDoneReading()) {
                char c = _text[_cursor++];
                if (c == '{') {
                    return {TokenType::LBRACE, "{", _line, _pos};
                } else if (c == '}') {
                    return {TokenType::RBRACE, "}", _line, _pos };
                } else if (c == '\"') {
                    return lexString();
                } else if (c == ':') {
                    return {TokenType::COLON, ":", _line, _pos };
                } else if (c == ',') {
                    return {TokenType::COMMA, ",", _line, _pos };
                } else if (c == '[') {
                    return { TokenType::LBRACKET, "[", _line, _pos };
                } else if (c == ']') {
                    return { TokenType::RBRACKET, "]", _line, _pos };
                } else if (c == 't') {
                    return lexBool(c, "true");
                } else if (c == 'f') {
                    return lexBool(c, "false");
                } else if (c == 'n') {
                    return lexNull();
                } else if (c == '-' || c == '+' || std::isdigit(c)) {
                    return lexNumber(c);
                } else {
                    throw parse_exception(c + " is not a valid token!");
                }
            }
            return {TokenType::NONE, ""};
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
