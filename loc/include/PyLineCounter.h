#pragma once

#include "ILineCounter.h"

class PyLineCounter : ILineCounter
{
public:
    unsigned long CountLines(std::string path) override;
};
