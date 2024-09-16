#pragma once

#include <iostream>
#include <string>

struct args {
	size_t k;
	size_t n;
	size_t M;
};

[[nodiscard]] bool parse(int const& argc, char* const argv[], args& r);