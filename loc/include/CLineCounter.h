// Count the lines of code in a C file (works with almost any C-like language)

#pragma once

#include "ILineCounter.h"

class CLineCounter : public ILineCounter
{
public:
	unsigned long CountLines(const std::filesystem::path& path) override;
};
