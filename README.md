 jsonpp

jsonpp is a fast and easy to use JSON manipulation library written for C++14. It was written with API design and code readability 
in mind to allow manipulating JSON to be as easy as possible for developers and a pleasurable experience reading the source code.

## Usage
jsonpp is a single header/source library that can be easily integrated into any C++ environment. Simply put the header and source file into a include and source
directory recognized by your compiler and you are good to go.

## Examples
### loading and parsing the JSON
```
#include "jsonpp.h"
// ... load the JSON somehow as a string
// here we use in-place text as an example
auto text = 
 R"({ 
        "foo" : "bar"
   })";

// parse the JSON and return an in-memory DOM. If parsing fails, a json::parse_exception is thrown
auto obj = json::parse(text); 

// now that the DOM is yours, manipulate it in any way you choose
// for example, this returns the value mapped to "foo"
auto value = obj->getValue("foo");
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
* [CMake](https://cmake.org)
* [CATCH](https://github.com/catchorg/Catch2)
