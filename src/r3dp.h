#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
#define R3DP_EXPORT __declspec(dllexport)
#else
#define R3DP_EXPORT
#endif

R3DP_EXPORT void r3dp();
R3DP_EXPORT void r3dp_print_vector(const std::vector<std::string> &strings);
