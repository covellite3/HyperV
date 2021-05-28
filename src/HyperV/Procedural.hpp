/**
 * \author Asso Corentin
 * \Date April 22 2021
 * \Desc Bundle of randomness, noises and procedural methodes.
 */
#pragma once

#include "Util.hpp"

namespace HyperV {

namespace Procedural {
	/** RNG 1D */
	static inline float RNG(float x, float seed) {
		ASSERT(!std::isnan(x), "Input cannot be NaN.");
		ASSERT(!std::isinf(x), "Input cannot be infinite.");
		ASSERT(!std::isnan(seed), "Seed cannot be NaN.");
		ASSERT(!std::isinf(seed), "Seed cannot be infinite.");
		ASSERT(seed != 0, "Seed cannot be 0."); // Nothing will happened if seed equal 0.
	    auto value = Math::fract(std::sin(x*seed)*3450206.7354991f);
	    ASSERT(value <= 1.0f+Math::EPS, "RNG cannot be higher than 1.");
	   	ASSERT(value >= 0.0f-Math::EPS, "RNG cannot be lower than 0.");
	   	ASSERT(!std::isnan(value), "RNG cannot be NaN.");
		ASSERT(!std::isinf(value), "RNG cannot be infinite.");
	    return value;
	}

	/** Return a random vector of N dimension for a point in N-hyperspace. */
	template<size_t N, typename T, size_t I = 0>
	static inline constexpr T RNG_VEC(T worldPos, float seed)
	{
		if constexpr (I == N)
			return T();
		else {
			auto u = RNG_VEC<N, T, I+1>(worldPos, seed);
			ASSERT(!std::isnan(worldPos[I]), "World position cannot be NaN.");
			ASSERT(!std::isinf(worldPos[I]), "World position cannot be infinite.");
			u[I] = RNG(worldPos[I], seed+I*95643.45224f);
			return u;
		}
	}

	/** RNG for N dimensions. */
	template<size_t N, typename T>
	static inline float RNG(T v, float seed)
	{
		auto randomVec = RNG_VEC<N, T>(v, seed);
	    auto value = Math::fract(std::sin(Math::dot<N>(v, randomVec))*43758.5453123f);
	   	ASSERT(value <= 1.0f+Math::EPS, "RNG cannot be higher than 1.");
	   	ASSERT(value >= 0.0f-Math::EPS, "RNG cannot be lower than 0.");
	   	ASSERT(!std::isnan(value), "RNG cannot be NaN.");
		ASSERT(!std::isinf(value), "RNG cannot be infinite.");
	    return value;
	}

	/**
	 * Perlin noise in N dimensional space.
	 * - worldPos : Position in the world.
	 * - cellPerUnit : Number of cell per world's unit.
	 * - seed : Any random number except for 0.
	 * - N : Number of dimensions.
	 * - T : Type used for vectors.
	 */
	template<size_t N, typename T, typename E>
	static float PerlinNoise(const T& worldPos, const E& cellPerUnit, float seed) {
		assert(std::abs(seed) > 0.0000001); // Seed 0 is banned !
	    
	    auto buffer = Math::Mul<N>(worldPos, cellPerUnit);
	    std::array<  int, N> i = Math::vec_cast<N, std::array<  int, N>,   int>(Math::floor<N>(buffer));
	    std::array<float, N> f = Math::vec_cast<N, std::array<float, N>, float>(Math::fract<N>(buffer));

	    /** Generate a RNG for each verticices inside an n-hypercube. */
	    constexpr auto NVERTICES = Math::pow<2, N>();
	    std::array<float, NVERTICES> v;
	    auto it_v = v.begin();

	    std::array<size_t, N> forloop_limits;
		std::fill_n(forloop_limits.begin(), N, 2);
	    Misc::NestedForLoops<N>([&](const std::array<size_t, N>& coords) {
	        *it_v = RNG<N>(Math::vec_cast<N, std::array<float, N>, float>(Math::Add<N>(i, coords)), seed);
	        ++it_v;
	       	NFL_LAST_CALL;
	    }, forloop_limits);

	   	// Interpolate on all verticies :
	    auto noise = Math::nlerp<N>(v, Math::cubic_hermite_curve<N>(f));
	   	//auto noise = Math::nlerp<N>(v, Math::quintic_curve<N>(f));

	    ASSERT(noise <= 1.0f+Math::EPS, "Noise cannot be higher than 1.");
	   	ASSERT(noise >= 0.0f-Math::EPS, "Noise cannot be lower than 0.");
	   	return noise;
	}

#include "Procedural.inl"
}

} // namespace HyperV