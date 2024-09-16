#include "ArgParser.h"

#include <map>

[[nodiscard]] bool is_integer(const std::string& s) {
	auto it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

bool parse(int const& argc, char* const argv[], args& r) {
	using std::cout;
	using std::string;
	using std::stoll;
	using std::map;

	r = { 1,1 };

	size_t option_count = 0;
	auto options = map<string, bool>();
	for (size_t i = 1; i < argc; ++i) {

		if (argv[i][0] == '-') {
			auto option = string(&argv[i][1]);
			if (options.find(option) != options.end()) {
				cout << "ERROR: option \"" << argv[i] << "\" multiple occurences for option " << option_count << "\n";
				return false;
			}
			else {
				options[option] = true;
			}

			bool integer = true;
			if (option.compare("k") == 0) {
				auto tmp = string(argv[++i]);
				if (integer = is_integer(tmp) && tmp.compare("0") != 0) {
					r.k = stoll(tmp);
				}
			}
			else if (option.compare("n") == 0) {
				auto tmp = string(argv[++i]);
				if (integer = is_integer(tmp) && tmp.compare("0") != 0) {
					r.n = stoll(tmp);
				}
			}
			else {
				cout << "ERROR: invalid option \"" << argv[i] << "\" for option " << option_count << "\n";
				return false;
			}

			if (!integer) {
				cout << "ERROR: invalid parameter \"" << argv[i] << "\" for option " << option_count << "\n";
				return false;
			}

			++option_count;
		}
		else {
			cout << "ERROR: invalid option \"" << argv[i] << "\" for option " << option_count << "\n";
			return false;
		}
	}



	return true;
}