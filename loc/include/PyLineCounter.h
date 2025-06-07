#pragma once

#include "ILineCounter.h"

class PyLineCounter : public ILineCounter
{
public:
    unsigned long CountLines(const std::string& path) override;
};
