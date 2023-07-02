#pragma once

#include <stdint.h>

// Xoshiro256++ written by David Blackman and Sebastiano Vigna.
// This is adapted to generate 8 bit numbers

class Xoshiro256 {
private:
	uint64_t s[4];
	inline uint64_t rotl(const uint64_t x, int k) {
		return (x << k) | (x >> (64 - k));
	}
public:
	uint64_t randint(void) {
		const uint64_t result = rotl(s[0] + s[3], 23) + s[0];

		const uint64_t t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 45);

		return result;
	}

	Xoshiro256() {
		Xoshiro256(616328097, 108829579, 672443057, 239382367);
	}

	Xoshiro256(uint64_t s1, uint64_t s2, uint64_t s3, uint64_t s4) {
		s[0] = s1;
		s[1] = s2;
		s[2] = s3;
		s[3] = s4;
	}

	inline uint8_t randrange(uint64_t min, uint64_t max) {
		return static_cast<uint8_t>(min + (randint() % (max - min + 1)));
	}
};