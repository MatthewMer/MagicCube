#pragma once

namespace utils {
	struct lllint {
	public:
		lllint() = default;
		lllint(size_t const& higherhighint, size_t const& lowerhighint, size_t const& higherlowint, size_t const& lowerlowint);

		lllint& operator++();
		lllint operator++(int);
		lllint& operator--();
		lllint operator--(int);

	private:
		size_t data[4];
	};
}