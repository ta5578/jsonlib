#define CATCH_CONFIG_MAIN
#include "test/catch.hpp"
#include "jsonlib.h"

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
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
}

TEST_CASE("TestParsingObjectList")
{
    std::string text = 
    R"({
        "foo" : "bar" ,
        "abc" : "def"
    })";
    auto obj = json::parse(text);
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
    auto value2 = obj->getValue("abc");
    REQUIRE(value2 != nullptr);
}

TEST_CASE("TestParsingSingleElementArrayObject")
{
    std::string text =
    R"({
         "foo" : [ "bar" ]
    })";
    auto obj = json::parse(text);
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
}

TEST_CASE("TestParsingMultiElementArrayObject")
{
    std::string text =
    R"({
        "foo" : [ "bar" , "baz" ]
    })";
    auto obj = json::parse(text);
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
}