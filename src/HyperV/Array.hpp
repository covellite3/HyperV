/**
 * \author Asso Corentin
 * \Date April 17 2021
 * \Desc API for arrays.
 */
#pragma once

#include <array>
#include "Util.hpp"

namespace HyperV {

/**
 * Enum for the kind of ordering index used by this array.
 *
 * - S_ORDERING : Default, your typical ordering from n-space to 1D,
 * snake throught space. What you usually use.
 * To iterate with this indexing order, coordinnate of lower axis are
 * variating at higher frequency for for-loops.
 * Or you can just use an iterator.
 *
 * - Z_ORDERING : Also call Morton order, it's a space filling curve,
 * it do recursive Z patern to map the space to 1D. Perfect for binarytree,
 * quadtree or octree. Only work for square space, cubique space etc...
 * with side who are powers of two. Preserve more spacial and memory proxymity
 * than S_ORDERING.
 * To iterate this use an iterator.
 *
 * - U_ORDERING : It's the Hilbert's Curve, a space filling curve, it's use some
 * kind of U patern recursivly, work lke Z_ORDERING, also require to have equal side to
 * the power of two. It's more computational instensive than Z_ORDERING,
 * if you don't know what to use it for, just use Z_ORDERING.
 * To iterate this use an iterator.
 */
struct IndexingMode {
	enum Enum {S_ORDERING, Z_ORDERING, U_ORDERING};
};

/**
 * Static array of n dimension.
 * More memory efficient. Reduce cache-miss.
 * For power of two array, you can enable z-ordering to make
 * keep data close in space, also close in memory, perfect for
 * quadtree and octree. Decreasing futher cache-miss.
 */
template<IndexingMode::Enum INDEXING, typename T, size_t... DIMS>
class NArray {
public:
	/** Number of elements in the array. */
	static constexpr size_t SIZE = OpPack::Mul(DIMS...);

	/** Number of dimension for this array. */
	static constexpr size_t N = sizeof...(DIMS);

	/**
	 * Coordinate vector of indices
	 * with the same number of dimension.
	 * of the array.
	 */
	using Coordinates = std::array<size_t, N>;

private:
	/** One dimensional array. */
	std::array<T, SIZE> elements;

	/**
	 * Return index at given coordinate for S_ORDERING.
	 */
	template<size_t I, typename... Pack>
	static inline size_t S_IndexAtPack(size_t prevMulDim, size_t coord, Pack... coords)
	{
		if constexpr(I == (N-1)) {
			static_assert(sizeof...(coords) == 0, "Too much coords.");
			return coord*prevMulDim;
		} else {
			T buffer = prevMulDim*OpPack::Proj(I, DIMS...);
			return coord*prevMulDim + S_IndexAtPack<I+1, Pack...>(buffer, coords...);
		}
	}

	/**
	 * Return index at given coordinate for S_ORDERING.
	 */
	template<size_t I>
	static inline size_t S_IndexAtList(size_t prevMulDim, const Coordinates& coords)
	{
		if constexpr(I == (N-1))
			return coords[I]*prevMulDim;
		else {
			T buffer = prevMulDim*OpPack::Proj(I, DIMS...);
			return coords[I]*prevMulDim + S_IndexAtList<I+1>(buffer, coords);
		}
	}

public:
	/**
	 * Return index at given coordinate.
	 */
	template<typename... Pack>
	static inline size_t IndexAt(Pack... coords)
	{
		ASSERT(Math::Lower<N>(Coordinates{coords...}, Coordinates{DIMS...}), "Coordinates out of bounds.");
		static_assert(
			sizeof...(coords) == N,
			"You must give one coordinate for each dimension(s)."
		);
		if constexpr (INDEXING==IndexingMode::S_ORDERING) { return S_IndexAtPack<0>(1, coords...); }
		else if constexpr (INDEXING==IndexingMode::Z_ORDERING) { ASSERT(false, "Not implemented yet."); return 0; }
		else if constexpr (INDEXING==IndexingMode::U_ORDERING) { ASSERT(false, "Not implemented yet."); return 0; }
		else { ASSERT(false, "Unknow indexing."); return 0; }	
	}

	/**
	 * Return index at given coordinate.
	 */
	static inline size_t IndexAt(const Coordinates& coords)
	{
		ASSERT(Math::Lower<N>(coords, Coordinates{DIMS...}), "Coordinates out of bounds.");
		if constexpr (INDEXING==IndexingMode::S_ORDERING) { return S_IndexAtList<0>(1, coords); }
		else if constexpr (INDEXING==IndexingMode::Z_ORDERING) { ASSERT(false, "Not implemented yet."); return 0; }
		else if constexpr (INDEXING==IndexingMode::U_ORDERING) { ASSERT(false, "Not implemented yet."); return 0; }
		else { ASSERT(false, "Unknow indexing."); return 0; }	
	}

	/**
	 * Return coordinate for given index.
	 */
	template<size_t AXIS = N-1>
	static Coordinates CoordsFor(Coordinates& coords, size_t index)
	{
		ASSERT(index < SIZE, "Index out of bounds.");
 		if constexpr (AXIS == 0) {
			coords[AXIS] = index % OpPack::Proj(0, DIMS...);
		} else {
			size_t products = 1;
			for(int m = 0; m < AXIS; ++m)
				products *= WidthOf(m);
			coords[AXIS] = index / products;
			index -= (coords[AXIS] * products);
		}

		// Calculate value for next axis.
		if constexpr(AXIS > 0) CoordsFor<AXIS-1>(coords, index);

		// For conveniency we return the ref of the array.
		ASSERT(Math::Lower<N>(coords, Coordinates{DIMS...}), "Coordinates out of bounds.");
		return coords;
	}

	/**
	 * Return coordinate for given index.
	 */
	static inline Coordinates CoordsFor(size_t index)
	{
		Coordinates coords;
		return CoordsFor(coords, index);
	}

	/** Get pointer to element at given coordinate. */
	template<typename... Pack>
	inline T* GetPointerAt(Pack... coords)
	{
		return &elements[IndexAt(coords...)];
	}

	/** Get pointer to element at given coordinate. */
	template<typename... Pack>
	inline const T* GetPointerAt(Pack... coords) const
	{
		return &elements[IndexAt(coords...)];
	}

	/** Get pointer to element at given coordinate. */
	inline T* GetPointerAt(const Coordinates&  coords)
	{
		return &elements[IndexAt(coords)];
	}

	/** Get pointer to element at given coordinate. */
	inline const T* GetPointerAt(const Coordinates&  coords) const
	{
		return &elements[IndexAt(coords)];
	}

	/**
	 * Get width of the array on a given axes at compile time.
	 */
	template<size_t AXIS>
	constexpr static inline size_t WidthOf()
	{
		static_assert(
			AXIS < N,
			"Demanding the size of an axis of higher dimension from this array."
		);
		return OpPack::Proj(AXIS, DIMS...);
	}

	constexpr static inline size_t GetWidth()	{ return WidthOf<AXIS_X>(); }
	constexpr static inline size_t GetHeight()	{ return WidthOf<AXIS_Y>(); }
	constexpr static inline size_t GetDepth()	{ return WidthOf<AXIS_Z>(); }
	constexpr static inline size_t GetSpissitude()	{ return WidthOf<AXIS_W>(); }

	/**
	 * Get width of the array on a given axes at runtime.
	 */
	static size_t WidthOf(size_t axis)
	{
		ASSERT(axis < N, "Not so many axis for this array.");
		constexpr std::array<size_t, N> dims( {DIMS...} );
		return dims[axis];
	}

	/** Copy operator */
	NArray& operator= (const NArray& other)
	{
		return *this;
	}

	/** Accessing array as a one dimensional array. */
	inline T& operator[] (size_t i)
	{
		ASSERT(i < SIZE, "Index out of bound.");
		return elements[i];
	}

	/** Accessing array as a one dimensional array. */
	inline const T& operator[] (size_t i) const
	{
		ASSERT(i < SIZE, "Index out of bound.");
		return elements[i];
	}

	/** Get element at given coordinate. */
	inline T& operator() (const Coordinates&  coords)
	{
		return *GetPointerAt(coords);
	}

	/** Get element at given coordinate. */
	inline const T& operator() (const Coordinates&  coords) const
	{
		return *GetPointerAt(coords);
	}

	/** Get element at given coordinate. */
	template<typename... Pack>
	inline T& operator() (Pack... coords)
	{
		return *GetPointerAt(coords...);
	}

	/** Retun an iterator at the begin of the array. */
	inline auto begin()
	{
		return elements.begin();
	}

	/** Retun an iterator after the end of the array. */
	inline auto end()
	{
		return elements.end();
	}

	static_assert(
		!OpPack::HasZero(DIMS...),
		"An array cannot have a lenght of 0 on one of his dimensions."
	);

	static_assert(
		N > 0,
		"An array must be of dimension 1 or higher."
	);

	static_assert(
		INDEXING != IndexingMode::Z_ORDERING &&
		INDEXING != IndexingMode::U_ORDERING,
		"Z_ORDERING and U_ORDERING not implemented yet."
	);

	static_assert(
		INDEXING == IndexingMode::S_ORDERING ||
		INDEXING == IndexingMode::Z_ORDERING ||
		INDEXING == IndexingMode::U_ORDERING,
		"There is only 3 INDEXING mode, choose wisely. S, Z and U ordering."
	);

	static_assert(
		INDEXING == IndexingMode::S_ORDERING ||
		(
			(INDEXING == IndexingMode::Z_ORDERING || INDEXING == IndexingMode::U_ORDERING) &&
			OpPack::Eq(DIMS...) &&
			Math::IsPowerOfTwo(OpPack::Proj(0, DIMS...))
		),
		"For Z, and U indexing to work, all side must be equal, and be a power of two."
	);

};

#include "Array.inl"


// Definition of commons types :

// S ordered types definitions.
template<typename T, size_t... DIMS>
using SArray = NArray<IndexingMode::S_ORDERING, T, DIMS...>;

// Z ordered types definitions.
template<typename T, size_t... DIMS>
using ZArray = NArray<IndexingMode::Z_ORDERING, T, DIMS...>;

template<typename T, size_t SIZE>
using ZArray1D = ZArray<T, SIZE>;

template<typename T, size_t SIZE>
using ZArray2D = ZArray<T, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray3D = ZArray<T, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray4D = ZArray<T, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray5D = ZArray<T, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray6D = ZArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray7D = ZArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray8D = ZArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using ZArray9D = ZArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

// U ordered types definitions.
template<typename T, size_t... DIMS>
using UArray = NArray<IndexingMode::U_ORDERING, T, DIMS...>;

template<typename T, size_t SIZE>
using UArray1D = UArray<T, SIZE>;

template<typename T, size_t SIZE>
using UArray2D = UArray<T, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray3D = UArray<T, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray4D = UArray<T, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray5D = UArray<T, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray6D = UArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray7D = UArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray8D = UArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

template<typename T, size_t SIZE>
using UArray9D = UArray<T, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE, SIZE>;

} // namespace HyperV