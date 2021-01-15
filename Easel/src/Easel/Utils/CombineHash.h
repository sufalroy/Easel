#pragma once
#include <cstddef>

#include <string>

namespace Easel {

	inline void HashCombine(std::size_t& seed) {}

	template <typename T, typename... Rest>
	inline void HashCombine(std::size_t& seed, const T& v, Rest... rest) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		HashCombine(seed, rest...);
	}
}
