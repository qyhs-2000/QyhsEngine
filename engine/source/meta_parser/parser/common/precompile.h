#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <mustache.hpp>
namespace fs = std::filesystem;
namespace Mustache = kainjow::mustache;
#include "meta/meta_data_config.h"
#include "clang-c/Index.h"