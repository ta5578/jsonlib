# jsonpp

jsonpp is a fast and easy to use JSON manipulation library written for C++14. It was written with API design and code clarity
in mind to allow accessing and manipulating JSON to be as easy as possible for C++ developers. It follows the language spec set forth at [JSON](https://www.json.org).

## Usage
jsonpp is a single header/source library that can be easily integrated into any C++ environment. Simply put the header and source file into a include and source directory recognized by your compiler. 

## Key Features
### Concise error reporting during JSON parsing
* This can allow for easier debugging when dealing with problematic JSON. One possible error message:
```
Expecting '<string>' at line 7:5 but found '}' instead!
```
This message is saying that at line `7` position `5`, a `string` token was expected but a `}` was found instead.

### Intuitive, consistent, and easy to use API.
* All parsing errors throw `json::parse_exception`s.
* All library functions live within the `json` namespace.
* See the `example` section below for an example of parsing JSON and getting values out of the DOM. For more complete examples, visit the `tests.cpp` file.

## Example
```
#include "jsonpp.hpp"
int main()
{
	// ... load the JSON somehow as a string
	// here we use in-place text as an example
	auto text =
	 R"({
		"foo" : "bar",
		"abc" : false,
		"obj" : {
		    "yay" : "nay",
		    "lol" : "bbq",
		    "num" : 1234.55
		},
		"arr" : [ 1, 2, 3, 4, 5]
	   })";

	// parse the JSON and return an in-memory DOM. If parsing fails, a json::parse_exception is thrown
	auto obj = json::parse(text);

	// you are the sole owner of the DOM. Now let's get some values out...
	auto barValue = obj->getStringValue("foo"); // "bar"

	auto boolValue = obj->getStringValue("abc"); // false

        // in case of missing values, you can provide a default that will be returned.
	// defaults are normally set to empty string, null, or false depending on type
	auto valueNotAvailable = obj->getStringValue("baz", "foobar"); // "foobar"

	// recursive value search
	auto lolValue = obj->getStringValue("lol"); // "bbq"

	// get the sub object
	auto subObj = obj->getObjectValue("obj");

	// get the number
	auto numValue = subObj->getNumberValue("num"); // 12345.55

	// you can even get the number recursively ;)
	numValue = obj->getNumberValue("num"); // 12345.55

	// handle the array
	auto arr = obj->getArrayValue("arr");

	// loop through the values in the array
	auto values = arr->getValues();
	for (auto v : values) {
	    if (v->isNumber()) {
		double num = static_cast<json::Number*>(v)->getValue();
	    }
	}

	// obj and all of its children are automatically cleaned up
}
```

## Building
jsonpp uses CMake as its build system to build the library and run the unit tests. For now, the library will be built as a _static_ library; future versions of this library may allow it to be built as a dynamic library.

For convenience, a `build` directory was included in the `.gitignore` file to allow you to build the library in three steps:

```
mkdir build
cd build
cmake ..
```

By default, CMake will generate *release* versions of the library. To enable debug builds, build with CMAKE_BUILD_TYPE set to "debug". Note that this will affect single generators like _Make_ and _Ninja_. Other generators like the _Visual Studio_ generator allow you to customize the configuration type once the generator itself is built.

```
cmake .. -DCMAKE_BUILD_TYPE=debug
```

## Libraries
* [CMake Build System](https://cmake.org)
* [CATCH Unit Testing](https://github.com/catchorg/Catch2)

## Credits
All example JSON strings in `tests.cpp` are taken with credit to [sitepoint-editors repository](https://github.com/sitepoint-editors/json-examples).
