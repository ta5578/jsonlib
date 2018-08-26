#define CATCH_CONFIG_MAIN
#include "test/catch.hpp"
#include "jsonpp.hpp"
#include <fstream>

#define JSONPP_DOUBLE_EQUALS(obj, name, expected) do {\
    auto target = Approx((expected)).epsilon(std::numeric_limits<double>::epsilon() * 100);\
    REQUIRE(obj->getNumberValue(name) == target);\
} while (0)

#define JSONPP_DOUBLE_EQUALS_INDEX(arr, index, expected) do {\
    auto target = Approx((expected)).epsilon(std::numeric_limits<double>::epsilon() * 100);\
    REQUIRE(arr->getNumberValue((index)) == target);\
} while (0)

static constexpr const char* DB_JSON =
R"(
{
  "clients": [
    {
      "id": "59761c23b30d971669fb42ff",
      "isActive": true,
      "age": 36,
      "name": "Dunlap Hubbard",
      "gender": "male",
      "company": "CEDWARD",
      "email": "dunlaphubbard@cedward.com",
      "phone": "+1 (890) 543-2508",
      "address": "169 Rutledge Street, Konterra, Northern Mariana Islands, 8551"
    },
    {
      "id": "59761c233d8d0f92a6b0570d",
      "isActive": true,
      "age": 24,
      "name": "Kirsten Sellers",
      "gender": "female",
      "company": "EMERGENT",
      "email": "kirstensellers@emergent.com",
      "phone": "+1 (831) 564-2190",
      "address": "886 Gallatin Place, Fannett, Arkansas, 4656"
    },
    {
      "id": "59761c23fcb6254b1a06dad5",
      "isActive": true,
      "age": 30,
      "name": "Acosta Robbins",
      "gender": "male",
      "company": "ORGANICA",
      "email": "acostarobbins@organica.com",
      "phone": "+1 (882) 441-3367",
      "address": "697 Linden Boulevard, Sattley, Idaho, 1035"
    },
    {
      "id": "59761c23acd38891373f3efd",
      "isActive": true,
      "age": 38,
      "name": "Lawrence Morrison",
      "gender": "male",
      "company": "OCTOCORE",
      "email": "lawrencemorrison@octocore.com",
      "phone": "+1 (863) 482-3587",
      "address": "798 Troutman Street, Motley, New Mexico, 216"
    },
    {
      "id": "59761c230a89b90a7f47c8e5",
      "isActive": true,
      "age": 29,
      "name": "Trudy Bennett",
      "gender": "female",
      "company": "XPLOR",
      "email": "trudybennett@xplor.com",
      "phone": "+1 (920) 520-3028",
      "address": "141 Richardson Street, Carrsville, Utah, 5923"
    }
  ]
}
)";

static constexpr const char* GOOGLE_MARKERS_JSON =
R"(
{
  "markers": [
    {
      "name": "Rixos The Palm Dubai",
      "position": [25.1212, 55.1535]
    },
    {
      "name": "Shangri-La Hotel",
      "location": [25.2084, 55.2719]
    },
    {
      "name": "Grand Hyatt",
      "location": [25.2285, 55.3273]
    }
  ]
}
)";

static constexpr const char* YOUTUBE_SEARCH_JSON =
R"(
{
  "kind": "youtube#searchListResponse",
  "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/PaiEDiVxOyCWelLPuuwa9LKz3Gk\"",
  "nextPageToken": "CAUQAA",
  "regionCode": "KE",
  "pageInfo": {
    "totalResults": 4249,
    "resultsPerPage": 5
  },
  "items": [
    {
      "kind": "youtube#searchResult",
      "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/QpOIr3QKlV5EUlzfFcVvDiJT0hw\"",
      "id": {
        "kind": "youtube#channel",
        "channelId": "UCJowOS1R0FnhipXVqEnYU1A"
      }
    },
    {
      "kind": "youtube#searchResult",
      "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/sbCCQ5dWx1zCQwp5fKHlrtFTFJ4\"",
      "id": {
        "kind": "youtube#video",
        "videoId": "IGYEtw94zMM"
      }
    },
    {
      "kind": "youtube#searchResult",
      "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/dKYVYE72ZdmqgW9jTeTQOLtgUsI\"",
      "id": {
        "kind": "youtube#video",
        "videoId": "nxa6TeeJQ1s"
      }
    },
    {
      "kind": "youtube#searchResult",
      "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/AWutzVOt_5p1iLVifyBdfoSTf9E\"",
      "id": {
        "kind": "youtube#video",
        "videoId": "Eqa2nAAhHN0"
      }
    },
    {
      "kind": "youtube#searchResult",
      "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/2dIR9BTfr7QphpBuY3hPU-h5u-4\"",
      "id": {
        "kind": "youtube#video",
        "videoId": "IirngItQuVs"
      }
    }
  ],
  "result": {
    "kind": "youtube#searchListResponse",
    "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/PaiEDiVxOyCWelLPuuwa9LKz3Gk\"",
    "nextPageToken": "CAUQAA",
    "regionCode": "KE",
    "pageInfo": {
      "totalResults": 4249,
      "resultsPerPage": 5
    },
    "items": [
      {
        "kind": "youtube#searchResult",
        "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/QpOIr3QKlV5EUlzfFcVvDiJT0hw\"",
        "id": {
          "kind": "youtube#channel",
          "channelId": "UCJowOS1R0FnhipXVqEnYU1A"
        }
      },
      {
        "kind": "youtube#searchResult",
        "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/sbCCQ5dWx1zCQwp5fKHlrtFTFJ4\"",
        "id": {
          "kind": "youtube#video",
          "videoId": "IGYEtw94zMM"
        }
      },
      {
        "kind": "youtube#searchResult",
        "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/dKYVYE72ZdmqgW9jTeTQOLtgUsI\"",
        "id": {
          "kind": "youtube#video",
          "videoId": "nxa6TeeJQ1s"
        }
      },
      {
        "kind": "youtube#searchResult",
        "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/AWutzVOt_5p1iLVifyBdfoSTf9E\"",
        "id": {
          "kind": "youtube#video",
          "videoId": "Eqa2nAAhHN0"
        }
      },
      {
        "kind": "youtube#searchResult",
        "etag": "\"m2yskBQFythfE4irbTIeOgYYfBU/2dIR9BTfr7QphpBuY3hPU-h5u-4\"",
        "id": {
          "kind": "youtube#video",
          "videoId": "IirngItQuVs"
        }
      }
    ]
  }
}
)";

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
    std::string text =R"({
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

TEST_CASE("TestLexingJSONForPositionAndLineNumber")
{
    std::string text =
    R"({
    "foo" : "bar",
    "baz" : true,
    "abc" : 123
    })";

    json::detail::Lexer lexer(text);
    
    auto tok = lexer.getToken();
    REQUIRE(tok.value == "{");
    REQUIRE(tok.line == 1);
    REQUIRE(tok.pos == 1);

    tok = lexer.getToken();
    REQUIRE(tok.value == "foo");
    REQUIRE(tok.line == 2);
    REQUIRE(tok.pos == 5);

    tok = lexer.getToken();
    REQUIRE(tok.value == ":");
    REQUIRE(tok.line == 2);
    REQUIRE(tok.pos == 11);

    tok = lexer.getToken();
    REQUIRE(tok.value == "bar");
    REQUIRE(tok.line == 2);
    REQUIRE(tok.pos == 13);

    tok = lexer.getToken();
    REQUIRE(tok.value == ",");
    REQUIRE(tok.line == 2);
    REQUIRE(tok.pos == 18);

    tok = lexer.getToken();
    REQUIRE(tok.value == "baz");
    REQUIRE(tok.line == 3);
    REQUIRE(tok.pos == 5);

    tok = lexer.getToken();
    REQUIRE(tok.value == ":");
    REQUIRE(tok.line == 3);
    REQUIRE(tok.pos == 11);

    tok = lexer.getToken();
    REQUIRE(tok.value == "true");
    REQUIRE(tok.line == 3);
    REQUIRE(tok.pos == 13);

    tok = lexer.getToken();
    REQUIRE(tok.value == ",");
    REQUIRE(tok.line == 3);
    REQUIRE(tok.pos == 17);

    tok = lexer.getToken();
    REQUIRE(tok.value == "abc");
    REQUIRE(tok.line == 4);
    REQUIRE(tok.pos == 5);

    tok = lexer.getToken();
    REQUIRE(tok.value == ":");
    REQUIRE(tok.line == 4);
    REQUIRE(tok.pos == 11);

    tok = lexer.getToken();
    REQUIRE(tok.value == "123");
    REQUIRE(tok.line == 4);
    REQUIRE(tok.pos == 13);

    tok = lexer.getToken();
    REQUIRE(tok.value == "}");
    REQUIRE(tok.line == 5);
    REQUIRE(tok.pos == 5);
}

TEST_CASE("TestParseInnerQuoteMarkString")
{
    std::string text =
    R"({
        "foo" : "b\"ar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b"ar)");
}

TEST_CASE("TestParseReverseSolidus")
{
    std::string text =
        R"({
        "foo" : "b\\ar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\ar)");
}

TEST_CASE("TestParseSolidus")
{
    std::string text =
        R"({
        "foo" : "b\/ar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b/ar)");
}

TEST_CASE("TestParseBackspace")
{
    std::string text =
        R"({
        "foo" : "b\bar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\bar)");
}

TEST_CASE("TestParseFormfeed")
{
    std::string text =
        R"({
        "foo" : "b\far"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\far)");
}

TEST_CASE("TestParseNewLine")
{
    std::string text =
        R"({
        "foo" : "b\nar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\nar)");
}

TEST_CASE("TestParseCarriageReturn")
{
    std::string text =
        R"({
        "foo" : "b\rar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\rar)");
}

TEST_CASE("TestParseTab")
{
    std::string text =
        R"({
        "foo" : "b\tar"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == R"(b\tar)");
}

TEST_CASE("TestParseUControl")
{
    std::string text =
    R"({
        "foo" : "\uDEAD"
    })";

    auto obj = json::parse(text);
    REQUIRE(obj->getStringValue("foo") == "DEAD");
}

TEST_CASE("TestParseUControlWithLessThan4HexadecimalDigits")
{
    std::string text =
    R"({
        "foo" : "\uABC"
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseUControlWithGreaterThan4HexadecimalDigits")
{
    std::string text =
    R"({
        "foo" : "\uABCDE"
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestParseUControlFollowedByNonHexadecimalCharacters")
{
    std::string text =
    R"({
        "foo" : "\ufgh"
    })";
    REQUIRE_THROWS_AS(json::parse(text), json::parse_exception);
}

TEST_CASE("TestRecursiveSearch")
{
    std::string text =
    R"({
        "foo" : {
            "abc" : true
        }
    })";
    auto obj = json::parse(text);
    REQUIRE(obj->getBoolValue("abc") == true);
}

TEST_CASE("TestIntegralNumberInValuePair")
{
    std::string text =
    R"({
        "foo" : 36,
        "bar" : true
    })";

    auto obj = json::parse(text);
    JSONPP_DOUBLE_EQUALS(obj, "foo", 36);
}

TEST_CASE("TestDB_JSON")
{
    auto obj = json::parse(DB_JSON);
    auto arr = obj->getArrayValue("clients");
    REQUIRE(arr != nullptr);
    REQUIRE(arr->size() == 5);
    
    auto client1 = arr->getObjectValue(0);
    REQUIRE(client1 != nullptr);
    REQUIRE(client1->getStringValue("id") == "59761c23b30d971669fb42ff");
    REQUIRE(client1->getBoolValue("isActive") == true);
    REQUIRE(client1->getNumberValue("age") == 36);
    REQUIRE(client1->getStringValue("name") == "Dunlap Hubbard");
    REQUIRE(client1->getStringValue("gender") == "male");
    REQUIRE(client1->getStringValue("company") == "CEDWARD");
    REQUIRE(client1->getStringValue("email") == "dunlaphubbard@cedward.com");
    REQUIRE(client1->getStringValue("phone") == "+1 (890) 543-2508");
    REQUIRE(client1->getStringValue("address") == "169 Rutledge Street, Konterra, Northern Mariana Islands, 8551");
}

TEST_CASE("TestGoogleMarkers_JSON")
{
    auto obj = json::parse(GOOGLE_MARKERS_JSON);
    REQUIRE(obj->size() > 0);
    
    auto arr = obj->getArrayValue("markers");
    REQUIRE(arr->size() == 3);
    
    auto marker2 = arr->getObjectValue(1);
    REQUIRE(marker2->getStringValue("name") == "Shangri-La Hotel");
    
    auto loc = marker2->getArrayValue("location");
    REQUIRE(loc->size() == 2);
    JSONPP_DOUBLE_EQUALS_INDEX(loc, 0, 25.2084);
    JSONPP_DOUBLE_EQUALS_INDEX(loc, 1, 55.2719);
}

TEST_CASE("TestYoutubeSearchResults_JSON")
{
    auto obj = json::parse(YOUTUBE_SEARCH_JSON);
    REQUIRE(obj->getStringValue("etag") == R"("m2yskBQFythfE4irbTIeOgYYfBU/PaiEDiVxOyCWelLPuuwa9LKz3Gk")");

    auto pageInfo = obj->getObjectValue("pageInfo");
    JSONPP_DOUBLE_EQUALS(pageInfo, "totalResults", 4249);
    JSONPP_DOUBLE_EQUALS(pageInfo, "resultsPerPage", 5);

    auto items = obj->getArrayValue("items");
    REQUIRE(items->size() == 5);
    auto allItems = items->getValues();
    for (auto i : allItems) {
        REQUIRE(i->isObject());
        auto o = static_cast<json::Object*>(i);
        REQUIRE(!o->getStringValue("kind").empty());
    }

    auto i1 = items->getObjectValue(0);
    REQUIRE(i1->getStringValue("kind") == "youtube#searchResult");
}