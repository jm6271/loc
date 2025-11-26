/*
 * This is a C++ header file for the testing purposes
 * It contains some random code for the tests to use
*/

#include <iostream>

class SomeRandomeTestClass
{
public:
    SomeRandomeTestClass() /* Test inline multiline style comments */
    {
        /* Print Hello World to stdout */ std::cout << "Hello, world!\n";
    }

    ~SomeRandomeTestClass()
    {
        std::cout << "Goodbye, world!\n";
    }
}

// Should be 13 lines of code in this file
