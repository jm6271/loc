#pragma once

#include "ILineCounter.h"

class PyLineCounter : public ILineCounter
{
public:
    unsigned long CountLines(const std::filesystem::path& path) override;
};
