/**
 * \author Asso Corentin
 * \Date April 17 2021
 * \Desc General stuff.
 */
#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

#include <Core/Types.hpp>

namespace Ra { namespace Gui {} namespace Core { namespace Geometry {} namespace Utils {} } }
using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Core::Geometry;
using namespace Ra::Core::Utils;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long	uint64;
typedef unsigned long long	uint128;

typedef char	int8;
typedef short	int16;
typedef int 	int32;
typedef long	int64;
typedef long long	int128;

/** Say if we do 'a-b' it will cause an overflow. */
/*#define WILL_OVERFLOW_ON_ADD(a, b) ( \
	is_unsigned<decltype(a-b)>(a) && (a > b) \
)*/

/** Say if we do 'a-b' it will cause an overflow. */
/*#define WILL_OVERFLOW_ON_SUB(a, b) ( \
	is_unsigned<decltype(a-b)>(a) && (a < b) \
)*/

// TODO When assert will work with cmake
#define WILL_OVERFLOW_ON_ADD(a, b) ( \
	false \
)

// TODO When assert will work with cmake
/** Say if we do 'a-b' it will cause an overflow. */
#define WILL_OVERFLOW_ON_SUB(a, b) ( \
	false \
)


/** TODO make a proper static_warning system. */
#define static_warning(predicat, msg) static_assert(predicat, msg)

/** For stringify __LINE__ */
#define STR_HELPER(x) #x

/** For stringify __LINE__ */
#define STR(x) STR_HELPER(x)

/** Assert with msg. */
#define ASSERT(predicat, msg) HyperV::custom_assert(predicat, "\x1B[91mASSERTION_FAILED \x1B[95m: In \x1B[91m'" __FILE__ "'\x1B[95m::\x1B[91m" STR(__FUNCTION__) "\x1B[95m at line \x1B[91m" STR(__LINE__) "\x1B[95m -> \x1B[93m" msg "\033[0m")

namespace HyperV {

/** Functon for custom assert with msg. */
void custom_assert(bool predicat, const char* msg);

template<typename T, size_t N>
using Vector = Eigen::Matrix<T, N, 1>;

template<size_t N>
using VectorNf = Vector<float, N>;


/** Give an enum for each axis so we can add more semantics to code. */
enum E_AXIS {AXIS_X=0, AXIS_Y=1, AXIS_Z=2, AXIS_W=3};

/** Utility class for operation on template's parameters pack. */
namespace OpPack {
/**
 * Get a mask representing a list of bool.
 */
template<typename... Pack>
static constexpr auto BitMaskEncoding(Pack... p)
{
	ASSERT(sizeof...(p)+1 <= 64, "You have more than 64 bool to store inside a 64 bit unsigned integer bitmask.");

	uint64 bitmask = 0;
	for(uint8 i = 0; i < sizeof...(p); ++i) {
		bool t = Proj(i, p...);
		ASSERT(t == 0 || t == 1, "Invalid 'bool'. 'bool' must be either 0 or 1. Not something else.");
		bitmask |= ((uint64) t) << i;
	}
	return bitmask;
}

/** 
 * Get bool from a bit mask.
 */
template<typename T>
static inline constexpr bool BitMaskDecoding(uint8 i, T bitmask)
{
	static_assert(std::is_unsigned<T>(), "Bit mask must be stored inside an unsigned integer.");
	ASSERT(i < sizeof(T)*8, "Index out of bound of this mask.");
	return (bitmask & (0b1 << i)) >> i;
}


/** Proj n element of parameter pack, starting from the end. (Singleton)*/
template<typename T>
static inline constexpr size_t InvProj(size_t index, T t)
{
	return t;
}

/** Proj n element of parameter pack, starting from the end.*/
template<typename T, typename... Pack>
static inline constexpr size_t InvProj(size_t index, T t, Pack... p)
{
	if (sizeof...(p) == index) return t;
	else return InvProj(index, p...);
}

/** Proj n element of parameter pack. (Singleton) */
template<typename T>
static inline constexpr size_t Proj(size_t index, T t)
{
	return t;
}

/** Proj n element of parameter pack. */
template<typename T, typename... Pack>
static inline constexpr size_t Proj(size_t index, T t, Pack... p)
{
	return InvProj(sizeof...(p)-index, t, p...);
}

/** Say if parameter pack elements are equal to 0. (Singleton)*/
template<typename T>
static inline constexpr bool HasZero(T t) { return t == 0; }

/** Say if parameter pack elements are equal to 0.*/
template<typename T, typename... Pack>
static inline constexpr bool HasZero(T t, Pack... p) { return t == 0 || HasZero(p...); }

/** Say if some of parameter pack elements are equal to some value. (Singleton)*/
template<typename T>
static inline constexpr bool HasValue(T v, T t) { return t == v; }

/** Say if some of parameter pack elements are equal to some value.*/
template<typename T, typename... Pack>
static inline constexpr bool HasValue(T v, T t, Pack... p) { return t == v || HasValue(v, p...); }

/** Say if all of parameter pack elements are equal to some value. (Singleton)*/
template<typename T>
static inline constexpr bool AllEqTo(T v, T t) { return t == v; }

/** Say if all of parameter pack elements are equal to some value.*/
template<typename T, typename... Pack>
static inline constexpr bool AllEqTo(T v, T t, Pack... p) { return t == v && AllEqTo(v, p...); }

/** Perfom equal on parameter pack. (Singleton)*/
template<typename T>
static inline constexpr T Eq(T t) { return t; }

/** Perfom equal on parameter pack.*/
template<typename T, typename... Pack>
static inline constexpr T Eq(T t, Pack... p) { return t == Eq(p...); }

/**
 * Give number of different typename in parameter pack. (NULL)
 * EXPERIMENTAL !
 */
template<typename T>
static inline constexpr T CountTypes(void) { return 0; }

/**
 * Give number of different typename in parameter pack. (Singleton)
 * EXPERIMENTAL !
 */
template<typename T>
static inline constexpr T CountTypes(T t) { return 1; }

/**
 * Give number of different typename in parameter pack.
 * EXPERIMENTAL !
 */
template<typename T, typename... Pack>
static inline constexpr T CountTypes(T t, Pack... p) { return 1 + CountTypes(p...); }

/** Perfom addition on parameter pack. (Singleton)*/
template<typename T>
static inline constexpr T Add(T t) { return t; }

/** Perfom addtion on parameter pack.*/
template<typename T, typename... Pack>
static inline constexpr T Add(T t, Pack... p) { return t + Add(p...); }

/** Perfom substraction on parameter pack. (Singleton)*/
template<typename T>
static inline constexpr T Sub(T t) { return t; }

/** Perfom substraction on parameter pack.*/
template<typename T, typename... Pack>
static inline constexpr T Sub(T t, Pack... p) { return t - Sub(p...); }

/** Perfom multiplication on parameter pack. (Singleton)*/
template<typename T>
static inline constexpr T Mul(T t) { return t; }

/** Perfom multiplication on parameter pack.*/
template<typename T, typename... Pack>
static inline constexpr T Mul(T t, Pack... p) { return t * Mul(p...); }

/** Perfom division on parameter pack. (Singleton)*/
template<typename T>
static inline constexpr T Div(T t) { return t; }

/** Perfom division on parameter pack.*/
template<typename T, typename... Pack>
static inline constexpr T Div(T t, Pack... p) { return t / Div(p...); }
}

namespace Math {

/** Epsilon. */
constexpr float EPS = 0.0000000000000000001f;


template<typename T>
static inline bool IsEven(T x) {
	static_assert(
		!std::is_same<T, float>() && !std::is_same<T, double>(),
		"Thoses bitwise operation for calculating if number is an power of two only work for integers."
	);
	return !(x&0b1);
}

template<typename T>
static inline bool IsOdd(T x) { 
	static_assert(
		!std::is_same<T, float>() && !std::is_same<T, double>(),
		"Thoses bitwise operation for calculating if number is an power of two only work for integers."
	);
	return x&0b1;
}


template<typename T>
static inline bool IsPowerOfTwo(T x)
{
	static_assert(
		!std::is_same<T, float>() && !std::is_same<T, double>(),
		"Thoses bitwise operation for calculating if number is an power of two only work for integers."
	);
	return (x&(x-1))==0;
}

/**
 * Constexpr version of pow for unsigned integer.
 * A - Base.
 * B - Exponent.
 */
template<uint64 A, uint64 B>
static inline constexpr uint64 pow(void)
{
	if(A == 2) { // Power of two in binary are easy to compute.
		return 0b1 << B;
	} else { // Compute power for any other base.
		if constexpr (B == 0) return 1;
		else if constexpr (B == 1) return A;
		else return A * pow<A, B-1>();
	}
}

/**
 * Get subset [L, U[ from set.
 * T - Type of the elements of the set.
 * L - Lower bound.
 * U - Upper bound.
 * N - Lenght of set.
 * M - Lenght of subset. (Don't specify it !)
 * I - Iteration. (Don't specify it !)
 */
template<typename T, size_t L, size_t U, size_t N, size_t M = U-L, size_t I = 0>
static inline std::array<T, M> Subset(const std::array<T, N>& set) {
	static_assert(L != U, "Cannot make null subset.");
	static_assert(L < U, "Lower bound must be inferior to upper bound.");
	static_assert(M == U-L, "Lenght subset is wrong.");
	static_assert(M <= N, "Subset's lenght must be lower or equal to set's lenght.");

	if constexpr (I == M) {
		return std::array<T,M>();
	} else {
		auto subset = Subset<T,L,U,N,M,I+1>(set);
		static_assert(I < N, "Out of set.");
		static_assert(I < M, "Out of subset.");
		subset[I] = set[I+L];
		return subset;
	}
}


/** Convert degree to radian */
template<typename T>
static inline T DegreeToRadian(const T& deg) { return deg*((T)M_PI)/((T)180); }

/** Absolute value. */
template<typename T>
static inline auto abs(const T& t)
{
	if(t < 0) return -t;
	else return t;
}

/** Absolute values for each element of array or vector. */
template<size_t N, typename T, size_t I = 0>
static inline auto abs(const T& v)
{
	if constexpr (I == N)
		return T();
	else {
		T u = abs<N,T,I+1>(v);
		u[I] = abs(v[I]);
		return u;
	}
}


/** Lenght of a vector. */
template<size_t N, typename T, size_t I = 0>
static inline float lenght(const T& v)
{
	if constexpr (I == 0) {
		return std::sqrt(lenght<N,T,I+1>(v));
	}
	else if constexpr (I == N) {
		return v[I-1]*v[I-1];
	}
	else {
		return v[I-1]*v[I-1] + lenght<N,T,I+1>(v);
	}
}

/** Normalize vector, with precomputed inverse of the lenght of the vector. */
template<size_t N, typename T, size_t I = 0>
static inline T normalize(const T& v, const float invLenghtVec)
{
	ASSERT(!std::isnan(invLenghtVec), "Inverse lenght vector cannot be NaN.");
	ASSERT(!std::isinf(invLenghtVec), "Inverse lenght vector cannot be infinite.");
	if constexpr (I == N)
		return T();
	else {
		T u = normalize<N,T,I+1>(v, invLenghtVec);
		u[I] = v[I]*invLenghtVec;
		return u;
	}
}

/** Normalize vector distance. */
template<size_t N, typename T>
static inline T normalize(const T& v)
{
	auto l = lenght<N,T>(v);
	ASSERT(l > 0, "Lenght of vector cannot be null or negatif.");
	return normalize<N,T>(v, 1.0f/l);
}

/** Euclidian distance. */
template<size_t N, typename T, size_t I = 0>
static inline float euclidian_dist(const T& v, const T& u)
{
	if constexpr (I == 0) {
		return std::sqrt(euclidian_dist<N,T,I+1>(v,u));
	}
	else if constexpr (I == N) {
		auto buf = v[I-1] - u[I-1];
		return buf*buf;
	}
	else {
		auto buf = v[I-1] - u[I-1];
		return buf*buf + euclidian_dist<N,T,I+1>(v,u);
	}
}

/** Cubic Hermite Curve. For lerps's gradiants. */
template<typename T>
static inline auto cubic_hermite_curve(const T& f) { return f*f*(3.0f-2.0f*f); }

/** Cubic Hermite Curve for vector or array. For lerps's gradiants.  */
template<size_t N, typename T, size_t I = 0>
static inline auto cubic_hermite_curve(const T& f)
{
	if constexpr (I == N) return T();
	else {
		auto buf = cubic_hermite_curve<N,T,I+1>(f);
		buf[I] = cubic_hermite_curve(f[I]);
		return buf;
	}
}

/* Quintic interpolation curve For lerps's gradiants.  */
template<typename T>
static inline auto quintic_curve(const T& f) { return f*f*f*(f*(f*6.0f-15.0f)+10.0f); }

/* Quintic interpolation curve for vector or array. For lerps's gradiants.  */
template<size_t N, typename T, size_t I = 0>
static inline auto quintic_curve(const T& f)
{
	if constexpr (I == N) return T();
	else {
		auto buf = quintic_curve<N,T,I+1>(f);
		buf[I] = quintic_curve(f[I]);
		return buf;
	}
}


/** Linear interpolation of scalars. */
template<typename A, typename B>
static inline auto lerp(const A& a, const B& b, float x)
{
	ASSERT(x >= 0, "Gradiant is lower than 0.");
	ASSERT(x <= 1, "Gradiant is higher than 1.");
	return a*(1.0f-x) + b*x;
}

/**
 * Linear interpolation for N dimension.
 * N - Number of dimension.
 * A - type of value to interpolate.
 * B - type of list of gradiant.
 * M - number of value to interpolate in the set. (Don't specify it !)
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename A, typename B, size_t M = pow<2, N>(), size_t I = 0>
static inline auto nlerp(const std::array<A, M>& values, const B& gradients)
{
	if constexpr(I == 0) static_assert(M == pow<2, N>(), "Wrong initialisation.");
	if constexpr(M == 2) {
		return lerp(values[0], values[1], (float)gradients[N-I-1]);
	} else {
		return lerp(
			nlerp<N,A,B,M/2,I+1>(Subset<A,0,M/2,M>(values), gradients),
			nlerp<N,A,B,M/2,I+1>(Subset<A,M/2,M,M>(values), gradients),
			gradients[N-I-1]
		);
	}
}

/** Get min value. */
template<typename T>
static inline T min(T a, T b) { return (a < b) ? a : b; }

/** Get max value. */
template<typename T>
static inline T max(T a, T b) { return (a < b) ? b : a; }

/** Born value in between two values. */
template<typename T>
static inline T clamp(T a, T inf = 0, T sup = 1) { return min(max(a, inf), sup); }

/** Round value n digit after coma. */
template<typename T>
static inline T round(T a, uint64 n) {
	T p = std::pow(10, n);
	int64 buff = a*p;
	return buff/p;
}

/** Floor scalar. */
static inline float floor(float x)
{
	return std::floor(x);
}

/** Floor on vector or array. */
template<size_t N, typename T, size_t I = 0>
static inline T floor(const T& v)
{
	if constexpr (I == N)
		return T();
	else {
		T u = floor<N, T, I+1>(v);
		u[I] = floor(v[I]);
		return u;
	}
}

/** Fract on scalar. */
static inline float fract(float x)
{
	ASSERT(!std::isnan(x), "Input is NaN.");
	ASSERT(!std::isinf(x), "Input is infinite.");
	auto f = x-floor(x);
	ASSERT(f <= 1+EPS, "Fract cannot be higher than 1.");
	ASSERT(f >= 0-EPS, "Fract cannot be lower than 0.");
	return f;
}

/** Fract on vector or array. */
template<size_t N, typename T, size_t I = 0>
static inline T fract(const T& v)
{
	static_warning(
		std::is_floating_point<decltype(fract(v[I]))>(),
		"fract must return a floating point."
	);
	if constexpr (I == N)
		return T();
	else {
		auto u = fract<N, T, I+1>(v);
		u[I] = fract(v[I]);
		return u;
	}

}

/** Dot product of two vectors or arrays. */
template<size_t N, typename T, typename E, size_t I = 0>
static inline float dot(const T& a, const E& b)
{
	/*static_warning(
		std::is_floating_point<decltype(a[I]*b[I])>(),
		"Dot product is not manipulating floating point."
	);*/
	if constexpr (I == N)
		return 0;
	else
		return a[I]*b[I] + dot<N, T, E, I+1>(a, b);
}

/** Multiply two vectors or arrays. */
template<size_t N, typename T, typename E, size_t I = 0>
static inline T Mul(const T& a, const E& b)
{
	if constexpr (I == N)
		return T();
	else {
		auto u = Mul<N, T, E, I+1>(a, b);
		u[I] = a[I]*b[I];
		return u;
	}
}

/** Multiply a vectors or an arrays by a scalar. */
template<size_t N, typename T, typename S, size_t I = 0>
static inline T MulScalar(const T& a, const S& b)
{
	if constexpr (I == N)
		return T();
	else {
		auto u = MulScalar<N,T,S,I+1>(a, b);
		u[I] = a[I]*b;
		return u;
	}
}

/** Add two vectors or arrays. */
template<size_t N, typename T, typename E, size_t I = 0>
static inline T Add(const T& a, const E& b)
{
	if constexpr (I == N)
		return T();
	else {
		auto u = Add<N, T, E, I+1>(a, b);
		ASSERT(!WILL_OVERFLOW_ON_ADD(a[I], b[I]), "Will overflow on add.");
		u[I] = a[I]+b[I];
		return u;
	}
}

/** Subtract two vectors or arrays. */
template<size_t N, typename T, typename E, size_t I = 0>
static inline T Sub(const T& a, const E& b)
{
	static_assert(std::is_unsigned<decltype(a[I]-b[I])>(), "Undefined behavior.");
	if constexpr (I == N)
		return T();
	else {
		auto u = Sub<N, T, E, I+1>(a, b);
		ASSERT(!WILL_OVERFLOW_ON_SUB(a[I], b[I]), "Will overflow on sub.");
		u[I] = a[I]-b[I];
		return u;
	}
}

/**
 * Add a vectors or an arrays with a scalar.
 * Can only add positive number.
 */
template<size_t N, typename T, typename S, size_t I = 0>
static inline T AddScalar(const T& a, const S& b)
{
	if constexpr (I == N)
		return T();
	else {
		auto u = AddScalar<N,T,S,I+1>(a, b);
		ASSERT(!WILL_OVERFLOW_ON_ADD(a[I], b), "Will overflow on add.");
		u[I] = a[I]+b;
		return u;
	}
}

/**
 * Subtract a vectors or an arrays with a scalar.
 * Can only sub signed number.
 */
template<size_t N, typename T, typename S, size_t I = 0>
static inline T SubScalar(const T& a, const S& b)
{
	if constexpr (I == N)
		return T();
	else {
		auto u = SubScalar<N,T,S,I+1>(a, b);
		ASSERT(!WILL_OVERFLOW_ON_SUB(a[I], b), "Will overflow on sub.");
		u[I] = a[I] - b;
		return u;
	}
}

/*
 * Cast vector type T to vector type R,
 * and cast all scalars values into R as C type.
 * N is the number of dimensions of the vector.
 */
template<size_t N, typename R, typename C, typename T, size_t I = 0>
static inline R vec_cast(const T& a)
{
	if constexpr (I == N)
		return R();
	else {
		auto u = vec_cast<N, R, C, T, I+1>(a);
		u[I] = (C)a[I];
		return u;
	}
}

/**
 * Take a vector or an array and say if all elements equal to some value.
 * N - Size vector or array.
 * T - Type of value.
 * V - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename T, typename V, size_t I = 0>
static inline bool AllEqTo(const T& value, const V& vec)
{
	if constexpr (I == N)
		return true;
	else {
		return vec[I] == value && AllEqTo<N,T,V,I+1>(value, vec);
	}
}

/**
 * Take a vector or an array and say if at least one elements is equal to some value.
 * N - Size vector or array.
 * T - Type of value.
 * V - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename T, typename V, size_t I = 0>
static inline bool HasValue(const T& value, const V& vec)
{
	if constexpr (I == N)
		return false;
	else {
		return (vec[I] == value) || HasValue<N,T,V,I+1>(value, vec);
	}
}

/**
 * Say if at least on one axis they are equal.
 * N - Size vector or array.
 * V - Type of vector or array
 * U - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename V, typename U, size_t I = 0>
static inline bool Match(const V& v, const U& u)
{
	if constexpr (I == N)
		return false;
	else {
		return v[I] == u[I] || Match<N,V,U,I+1>(v, u);
	}
}

/**
 * Say all elements of v are lower than element of u
 * N - Size vector or array.
 * V - Type of vector or array
 * U - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename V, typename U, size_t I = 0>
static inline bool Lower(const V& v, const U& u)
{
	if constexpr (I == N)
		return true;
	else {
		return v[I] < u[I] && Lower<N,V,U,I+1>(v, u);
	}
}

/**
 * Say all elements of v are higher than element of u
 * N - Size vector or array.
 * V - Type of vector or array
 * U - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename V, typename U, size_t I = 0>
static inline bool Higher(const V& v, const U& u)
{
	if constexpr (I == N)
		return true;
	else {
		return v[I] > u[I] && Higher<N,V,U,I+1>(v, u);
	}
}

/**
 * Say all elements of v are equals than element of u
 * N - Size vector or array.
 * V - Type of vector or array
 * U - Type of vector or array
 * I - Iteration. (Don't specify it !)
 */
template<size_t N, typename V, typename U, size_t I = 0>
static inline bool Equal(const V& v, const U& u)
{
	if constexpr (I == N)
		return true;
	else {
		return v[I] == u[I] && Equal<N,V,U,I+1>(v, u);
	}
}

} // namespace Math

namespace Misc {

/** To use inside neasted for loops's lambda function to simulate 'break' */
#define NFL_BREAK return true

/** To use inside neasted for loops's lambda function to simulate 'continue' */
#define NFL_CONTINUE return false

/**
* To use inside neasted for loops's lambda function has
* the last instruction used inside the function
*/
#define NFL_LAST_CALL return false

/**
 * Refactoring of NestedForLoops. Pack version.
 * - N iteration
 * - N number of dimension
 */
template<size_t I, size_t N, typename F, typename... Pack>
inline bool _NestedForLoops(std::array<size_t, N>& coords, F lambda, Pack... sizes) {
	static_assert(N >= 1, "Must have at least one dimension.");
	static_assert(I < N, "Must have less iteration than dimension.");
	for(coords[N-1-I] = 0; coords[N-1-I] < OpPack::Proj(N-1-I, sizes...); ++coords[N-1-I]) {
		if constexpr (I < N-1) {
			if(_NestedForLoops<I+1, N>(coords, lambda, sizes...)) return true;
		}
		else {
			if(lambda(coords)) return true;
		}
	}
	return false;
}

/**
 * This version doesn't use std::array, but parameters pack instead.
 * Simulate N nestead for loop. Great for working on N dimensional space.
 * If lambda function return true, quit loops else keep looping, it kinda like
 * simulated 'break;' for neasted for loops.
 * Return true if we encounter a 'return true;' during excution of lambda function
 * who is equivalent to 'break;' in this case. Return false otherwise.
 * Doing 'return false;' inside the lambda function is equivalent to doing 
 * 'continue;' in your regular nestead loops.
 * Lower dimension change at higher frequency, it is to match the S_ORDERING of
 * SArray, and therefor lower cache miss.
 * It would be equivalent to this pseudo code at dimension 4 :
 * 		for(w) for(z) for(y) for(x) lambda(...);
 * coords[0] would be iterator an X loop, coords[1] for Y etc...
 */
template<typename F, typename T, typename... Pack>
bool NestedForLoops(F lambda, T size, Pack... sizes) {
	std::array<size_t, sizeof...(sizes)+1> coords{0};
	return _NestedForLoops<0, sizeof...(sizes)+1>(coords.data(), lambda, size, sizes...);
}

/**
 * Refactoring of NestedForLoops. std::array version.
 * - N iteration
 * - N number of dimension
 */
template<size_t I = 0, size_t N, typename F>
inline bool _NestedForLoops(std::array<size_t, N>& coords, F lambda, const std::array<size_t, N>& sizes) {
	static_assert(N >= 1, "Must have at least one dimension.");
	static_assert(I < N, "Must have less iteration than dimension.");
	for(coords[N-1-I] = 0; coords[N-1-I] < sizes[N-1-I]; ++coords[N-1-I]) {
		if constexpr (I < N-1) {
			if(_NestedForLoops<I+1, N, F>(coords, lambda, sizes)) return true;
		}
		else {
			if(lambda(coords)) return true;
		}
	}
	return false;
}

/**
 * This version dosn't use parameters pack, but std::array instead.
 * Simulate N nestead for loop. Great for working on N dimensional space.
 * If lambda function return true, quit loops else keep looping, it kinda like
 * simulated 'break;' for neasted for loops.
 * Return true if we encounter a 'return true;' during excution of lambda function
 * who is equivalent to 'break;' in this case. Return false otherwise.
 * Doing 'return false;' inside the lambda function is equivalent to doing 
 * 'continue;' in your regular nestead loops.
 * Lower dimension change at higher frequency, it is to match the S_ORDERING of
 * SArray, and therefor lower cache miss.
 * It would be equivalent to this pseudo code at dimension 4 :
 * 		for(w) for(z) for(y) for(x) lambda(...);
 * coords[0] would be iterator an X loop, coords[1] for Y etc...
 */
template<size_t N, typename F>
bool NestedForLoops(F lambda, const std::array<size_t, N>& sizes) {
	std::array<size_t, N> coords{0};
	return _NestedForLoops<0, N>(coords, lambda, sizes);
}

} // namespace Misc


namespace HyperCube {

	/**
	 * Compile time : Get number of K-Cube composing a N-Cube.
	 */
	template<uint64 N, uint64 K>
	static constexpr size_t Composition()
	{
		if constexpr (K < 0 || N < K)
			return 0;
		else if constexpr (K == 0 && N == 0)
			return 1;
		else
			return 2*Composition<N-1, K>() + Composition<N-1, K-1>();
	}

	/**
	 * Runtime time : Get number of K-Cube composing a N-Cube.
	 */
	static size_t Composition(uint64 N, uint64 K)
	{
		if (K < 0 || N < K)
			return 0;
		else if (K == 0 && N == 0)
			return 1;
		else
			return 2*Composition(N-1, K) + Composition(N-1, K-1);
	}

};


#include "Util.inl"

} // namespace HyperV