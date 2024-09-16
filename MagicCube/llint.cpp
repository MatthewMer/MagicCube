#include "llint.h"

namespace utils {
	lllint::lllint(size_t const& higherhighint, size_t const& lowerhighint, size_t const& higherlowint, size_t const& lowerlowint) {
		data[0] = lowerlowint;
		data[1] = higherlowint;
		data[2] = lowerhighint;
		data[3] = higherhighint;
	}

	lllint& lllint::operator++() {
		for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
			auto tmp = data[i];
			++data[i];
			if (tmp < tmp + 1) {
				break;
			}
		}
		return *this;
	}

	lllint lllint::operator++(int) {
		lllint tmp(*this);
		operator++();
		return tmp;
	}

	lllint& lllint::operator--() {
		for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
			auto tmp = data[i];
			--data[i];
			if (tmp > tmp - 1) {
				break;
			}
		}
		return *this;
	}

	lllint lllint::operator--(int) {
		lllint tmp(*this);
		operator--();
		return tmp;
	}
}