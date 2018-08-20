# jsonlib

jsonlib is a fast and easy to use JSON manipulation library written for C++11. It was written with API design and code readability 
in mind to allow manipulating JSON to be as easy as possible for developers and a pleasurable experience reading the source code.

## Usage
jsonlib is a single header/source library that can be easily integrated into any C++ environment. Simply put the header and source file into a include and source
directory recognized by your compiler and you are good to go.

## Examples
### loading and parsing the JSON
```
#include "jsonlib.h"
// ... load the JSON somehow as a string
// here we use in-place text as an example
auto text = "{ \"foo\" : \"bar\" }"

// parse the JSON and return an in-memory DOM. If parsing fails, a json::parse_exception is thrown
auto obj = json::parse(text); 

// now that the DOM is yours, manipulate it in any way you choose
// for example, this returns the value mapped to "foo"
auto value = obj->getValue("foo");
```

## Building
jsonlib uses CMake as its build system. For now, the library will be built as a _static_ library; future versions of this library may allow it to be built as a dynamic library.

For convenience, a `build` directory was included in the `.gitignore` file to allow you to build the library in three steps:

```
mkdir build
cd build
cmake ..
```

Building the library in this fashion will also execute the unit tests.

## Libraries
* [CMake](https://cmake.org)
* [CATCH](https://github.com/catchorg/Catch2)