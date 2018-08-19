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
    json::detail::Lexer lexer("\"foo\"");
    auto tok = lexer.getToken();
    REQUIRE(tok.second == "foo");
}

TEST_CASE("TestLexingStringMissingTerminatorThrows")
{
    json::detail::Lexer lexer("\"foo");
    REQUIRE_THROWS_AS(lexer.getToken(), json::parse_exception);
}

TEST_CASE("TestStringValue")
{
    auto value = std::make_unique<json::String>("foo");
    REQUIRE(value->getValue() == "foo");
}

TEST_CASE("TestParsingStringPairObject")
{
    std::string text = "{ \"foo\" : \"bar\" }";
    auto obj = json::parse(text);
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
}

TEST_CASE("TestParsingObjectList")
{
    std::string text = 
    "{\
        \"foo\" : \"bar\" ,\
        \"abc\" : \"def\"\
    }";
    auto obj = json::parse(text);
    auto value = obj->getValue("foo");
    REQUIRE(value != nullptr);
    auto value2 = obj->getValue("abc");
    REQUIRE(value2 != nullptr);
}