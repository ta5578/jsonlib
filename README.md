# jsonpp

jsonpp is a fast and easy to use JSON manipulation library written for C++14. The library's primary goal is to have a simple and intuitive API for retrieving and manipulating JSON structures. It is also written with the developer in mind so the source code is easy to follow and change (I hope) :) jsonpp allows parsing and manipulating the JSON spec defined at [JSON](https://www.json.org).

## Usage
jsonpp is a single header/source library that can be easily integrated into any C++ environment. Simply put the header and source file into a include and source directory recognized by your compiler.

## Key Features
### Concise error reporting during JSON parsing
* This allows easier debugging when dealing with problematic JSON. One possible error message:
```
Expecting '<string>' at line 7:5 but found '}' instead!
```
This message is saying that at line `7` position `5`, a `string` token was expected but a `}` was found instead.

### Intuitive, consistent, and easy to use API.
* All parsing errors throw `json::parse_exception`s.
* All library functions live within the `json` namespace.
* Simple manipulation and retrieval of JSON values.
* See the `example` section below for an example of parsing JSON and getting values out of the DOM. For more complete examples, visit the `tests.cpp` file.

### No third party dependencies
* The library only depends on the C++14 standard library implementation for your system.

### Performance
Although performance is not the primary goal of `jsonpp`, it is still extremely fast. These measurements were taken on
on a Intel Core i5-4690 @ 3.50 GHz with 32.0 GB RAM Windows 10 Professional and represent the average of 10 runs on `Release` mode:

| JSON File Size (KB) | Time (us) |
| ------------------- | --------- |
| 8.93 | 151.5 |
| 88.789 | 1556.6 |
| 2354.68 | 39713.6 |
| 6744.7 | 114717.0 |

## Examples

Here is an example of parsing, getting values from the JSON DOM, adding values, and saving the JSON.

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
	std::unique_ptr<json::Object> obj = nullptr;
	try {
	    obj = json::parse(text);
	} catch (const json::parse_exception& e) {
	    // handle the exception
	}

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

	// add a value to the object
	obj->addValue("myCustomValue", std::make_unique<json::Number>(12));

	// save the updated DOM
	json::write(obj.get(), "my-json-file.json");

	// obj and all of its children are automatically cleaned up
}
```

## Building
jsonpp uses CMake as its build system to build the library, unit tests, and performance tests. For now, the library will be built as a _static_ library; future versions of this library may allow it to be built as a dynamic library.

For convenience, an empty `build` directory was included with the repo to allow you to build the library in two steps:

```
cd build
cmake ..
```

By default, CMake will generate *release* versions of the library. To enable debug builds, build with CMAKE_BUILD_TYPE set to "debug". Note that this will only affect CMake single generators like _Make_ and _Ninja_. Other generators like the _Visual Studio_ generator allow you to customize the configuration type once the generator itself is built.

```
cmake .. -DCMAKE_BUILD_TYPE=debug
```

## Credits
* [CMake](https://cmake.org)
* [CATCH](https://github.com/catchorg/Catch2)
* [json-generator](https://www.json-generator.com/)
* Thanks to [sitepoint-editors repository](https://github.com/sitepoint-editors/json-examples) for the example `json` strings for testing.