# loc

```loc``` is a tool for counting the lines of code in a
programming project. It is written in C++ and designed to be very fast.

## Suported languages

```loc``` supports the following languages:

- C
- C++
- C#
- F#
- Python

Any C-style language should work, and the program uses 
the C line counter for any file extension not recognized.

## Usage

```loc [options] path1 path2 ...```

### Options

```-h [ --help ]``` - Display help

```-j [ --jobs ]``` - Number of threads to use. Default is number of CPU cores available

```pattern``` - Semicolon-separated list of file extensions to search for

```path``` - Path to file. Can be a wildcard pattern

### Example

To count the lines of code in the ```loc``` codebase from 
the toplevel directory running on 10 threads, use this command:

```
loc -j 10 ./loc/*.cpp ./loc/include/*.h ./loc.tests/*.cpp
```

## Speed

```loc``` is designed to be as fast as possible using multi-threading.

Counting the total lines of code in the Boost C++ libraries
(over 4 million lines of code) takes about 5.1 seconds.
