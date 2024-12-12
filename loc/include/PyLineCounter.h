#pragma once

#include "ILineCounter.h"

class PyLineCounter : public ILineCounter
{
public:
    unsigned long CountLines(std::string& path) override;
};
