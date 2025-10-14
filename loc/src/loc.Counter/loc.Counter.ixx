module;

#include <filesystem>

export module loc.Counter;

import loc.Filesystem;

export import :CLineCounter;
export import :FSLineCounter;
export import :PyLineCounter;
