#include "jsonlib.h"
#include <cctype>

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

    namespace detail {
        
        Lexer::Lexer(const std::string& text)
            : _cursor(0), _text(text) {}

        bool Lexer::isDoneReading() const
        {
            return _cursor >= _text.size();
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

            return {TokenType::STRING, str};
        }


        Token Lexer::getToken() 
        {
            // skip white space
            while (std::isspace(_text[_cursor]) && !isDoneReading()) {
                ++_cursor;
            }

            while (!isDoneReading()) {
                char c = _text[_cursor++];
                if (c == '{') {
                    return {TokenType::LBRACE, "{"};
                } else if (c == '}') {
                    return {TokenType::RBRACE, "}"};
                } else if (c == '\"') {
                    return lexString();
                } else if (c == ':') {
                    return {TokenType::COLON, ":"};
                } else if (c == ',') {
                    return {TokenType::COMMA, ","};
                } else {
                    throw parse_exception(c + " is not a valid token!");
                }
            }
            return {TokenType::NONE, ""};
        }
    }

    static std::unique_ptr<json::Value> parseValue(detail::Lexer& lexer)
    {
        auto tok = lexer.getToken();
        // hard code strings for the moment
        if (tok.first != detail::TokenType::STRING) {
            throw parse_exception("Only expecting <string> type values but got " + tok.second + " instead!");
        }
        return std::make_unique<json::String>(tok.second);
    }

    /*
    Object definition:
    '{' -> "name" -> ':' -> value -> [,] -> '}'
    */
    static std::unique_ptr<json::Object> parseObject(detail::Lexer &lexer)
    {
        auto nextTok = lexer.getToken();
        if (nextTok.first != detail::TokenType::LBRACE) {
            throw parse_exception("Expecting '{' token but got " + nextTok.second + " instead!");
        }
                
        nextTok = lexer.getToken();

        // an empty object
        if (nextTok.first == detail::TokenType::RBRACE) {
            return std::make_unique<Object>();
        }

        auto obj = std::make_unique<Object>();
        bool isList = false;
        do {
            // name
            if (nextTok.first != detail::TokenType::STRING) {
                throw parse_exception("String token expected but got " + nextTok.second + " instead!");
            }
            auto name = nextTok.second;

            // colon
            nextTok = lexer.getToken();
            if (nextTok.first != detail::TokenType::COLON) {
                throw parse_exception("Expecting ':' token but got " + nextTok.second + " instead!");
            }

            // value
            auto value = parseValue(lexer);

            obj->addValue(name, std::move(value));
            
            // if there is a comma, we continue parsing the list
            // otherwise, we are at the end of the name/value pairs
            nextTok = lexer.getToken();
            if (nextTok.first == detail::TokenType::COMMA) {
                isList = true;
                nextTok = lexer.getToken(); // eat the comma
            } else {
                isList = false;
            }

        } while (isList);

        // closing brace
        if (nextTok.first != detail::TokenType::RBRACE) {
            throw parse_exception("Expecting '}' token but got " + nextTok.second + " instead!");
        }

        return obj;
    }

    std::unique_ptr<Object> parse(const std::string& text)
    {
        detail::Lexer lexer(text);
        return parseObject(lexer);
    }
}
