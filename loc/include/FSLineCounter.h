// Count the lines of code in a C file (works with almost any C-like language)

#pragma once

#include "ILineCounter.h"

class FSLineCounter : public ILineCounter
{
public:
	unsigned long CountLines(std::string& path) override;
};
