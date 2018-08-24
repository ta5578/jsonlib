#define CATCH_CONFIG_MAIN
#include "test/catch.hpp"
#include "jsonpp.hpp"

#define JSONPP_DOUBLE_EQUALS(obj, name, expected) do {\
    auto target = Approx((expected)).epsilon(std::numeric_limits<double>::epsilon() * 100);\
    REQUIRE(obj->getNumberValue(name) == target);\
} while (0)

TEST_CASE("TestEmptyStringThrows")
{
    REQUIRE_THROWS_AS(json::parse(""), json::parse_exception);
}

TEST_CASE("TestEmptyObjectBracketsDoesNotThrow")
{
    REQUIRE_NOTHROW(json::parse("{}"));
}

TEST_CASE("TestEmptyObjectBracketsDoesNotThrow_TestForSpaces")
{
    REQUIRE_NOTHROW(json::parse("   {   }  "));
}

TEST_CASE("TestMissingInitialObjectBracketThrows")
{
    REQUIRE_THROWS_AS(json::parse("}"), json::parse_exception);
}

TEST_CASE("TestMissingClosingObjectBracketThrows")
{
    REQUIRE_THROWS_AS(json::parse("{"), json::parse_exception);
}

TEST_CASE("TestLexingString")
{
    std::string text = R"("foo")";
    json::detail::Lexer lexer(text);
    auto tok = lexer.getToken();
    REQUIRE(tok.value == "foo");
}

TEST_CASE("TestLexingStringMissingTerminatorThrows")
{
    std::string text = R"("foo)";
    json::detail::Lexer lexer(text);
    REQUIRE_THROWS_AS(lexer.getToken(), json::parse_exception);
}

TEST_CASE("TestStringValue")
{
    auto value = std::make_unique<json::String>("foo");
    REQUIRE(value->getValue() == "foo");
}

TEST_CASE("TestParsingStringPairObject")
{
    std::string text =
    R"({
        "foo" : "bar"
    })";
    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == "bar");
}

TEST_CASE("TestParsingObjectList")
{
    std::string text = 
    R"({
        "foo" : "bar" ,
        "abc" : "def"
    })";
    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == "bar");
    REQUIRE(obj->getStringValue("abc") == "def");
}

TEST_CASE("TestParsingSingleElementArrayObject")
{
    std::string text =
    R"({
         "foo" : [ "bar" ]
    })";
    auto obj = json::parse(text);
    auto value = obj->getArrayValue("foo");
    REQUIRE(value != nullptr);
    REQUIRE(value->getStringValue(0) == "bar");
}

TEST_CASE("TestParsingMultiElementArrayObject")
{
    std::string text =
    R"({
        "foo" : [ "bar" , "baz" ]
    })";
    auto obj = json::parse(text);
    auto value = obj->getArrayValue("foo");
    REQUIRE(value != nullptr);
    REQUIRE(value->size() == 2);
    REQUIRE(value->getStringValue(0) == "bar");
    REQUIRE(value->getStringValue(1) == "baz");
}

TEST_CASE("TestLexBoolTrue")
{
    json::detail::Lexer lexer("true");
    auto token = lexer.getToken();
    REQUIRE(token.line == 1);
    REQUIRE(token.pos == 1);
    REQUIRE(token.value == "true");
    REQUIRE(token.type == json::detail::TokenType::JBOOL);
}

TEST_CASE("TestLexBoolFalse")
{
    json::detail::Lexer lexer("false");
    auto token = lexer.getToken();
    REQUIRE(token.line == 1);
    REQUIRE(token.pos == 1);
    REQUIRE(token.value == "false");
    REQUIRE(token.type == json::detail::TokenType::JBOOL);
}

TEST_CASE("TestParsingBools")
{
    std::string text =
    R"({
        "foo" : true,
        "bar" : false
    })";
    auto obj = json::parse(text);
    REQUIRE(obj->getBoolValue("foo") == true);
    REQUIRE(obj->getBoolValue("bar") == false);
}

TEST_CASE("TestLexNull")
{
    json::detail::Lexer lexer("null");
    auto token = lexer.getToken();
    REQUIRE(token.line == 1);
    REQUIRE(token.pos == 1);
    REQUIRE(token.value == "null");
    REQUIRE(token.type == json::detail::TokenType::JNULL);
}

TEST_CASE("TestParsingNull")
{
    std::string text =
    R"({
        "foo" : null
    })";
    auto obj = json::parse(text);
    REQUIRE(obj->getNullValue("foo") == nullptr);
}

TEST_CASE("TestParseDigitOnlyNumber")
{
    std::string text =
    R"({
        "foo" : 12345
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 12345);
}

TEST_CASE("TestParseNumberWithStartingNegativeAndDigitsOnly")
{
    std::string text =
    R"({
        "foo" : -12345
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", -12345);
}

TEST_CASE("TestParseNumberWithStartingPositiveAndDigitsOnly")
{
    std::string text =
    R"({
        "foo" : +12345
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", +12345);
}

TEST_CASE("TestParseIllegalNumberWithSignInMiddle")
{
    std::string text =
    R"({
        "foo" : 12-345
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseIllegalNumberSpaceSeparated")
{
    std::string text =
    R"({
        "foo" : 12 345
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseNumberDecimal")
{
    std::string text =
    R"({
        "foo" : 12345.67
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 12345.67);
}

TEST_CASE("TestParseNumberWithMultipleDecimals")
{
    std::string text =
    R"({
        "foo" : 12345.67.0
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseNumberWithExponentDecimals")
{
    std::string text =
        R"({
        "foo" : 12345.67e1
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 123456.7);
}

TEST_CASE("TestParseNumberWithPositiveExponentDecimals")
{
    std::string text =
    R"({
        "foo" : 12345.67e+1
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 123456.7);
}

TEST_CASE("TestParseNumberWithNegativeExponentDecimals")
{
    std::string text =
    R"({
        "foo" : 12345.67e-1
    })";
    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 1234.567);
}

TEST_CASE("TestParseNumberWithExponentMultipleDecimals")
{
    std::string text =
    R"({
        "foo" : 12345.67e1.0
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseNumberWithExponentBeforeDecimal")
{
    std::string text =
    R"({
        "foo" : 12345e1.67
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseNumberWithNonDigit")
{
    std::string text =
    R"({
        "foo" : 12345f.67
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}