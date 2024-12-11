// Count the lines of code in a C file (works with almost any C-like language)

#pragma once

#include "ILineCounter.h"

class CLineCounter : ILineCounter
{
public:
	unsigned long CountLines(std::string path) override;
};
