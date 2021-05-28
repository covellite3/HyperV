/**
 * \author Asso Corentin
 * \Date April 20 2021
 * \Desc Defintion of Chunk.
 */
#pragma once

#include "Array.hpp"
#include "VoxelSet.hpp"

#include <algorithm>

namespace HyperV {

/**
 * A chunk is relative to a terrain and tile it.
 * Chunk definition is complely versatile, they can be from any dimension,
 * size, type of VoxelSet, or mode of indexing storage.
 */
template<IndexingMode::Enum INDEXING, typename VOXELSET_SIZE_T, size_t... DIMS>
class Chunk {
public:
	/** Say the number of dimension for this chunk. */
	static constexpr size_t N = sizeof...(DIMS);

	/** Total number of voxel the chunk can store. */
	static constexpr size_t CAPACITY = OpPack::Mul(DIMS...); 

	/** Shortcut for the type of the array where are stored voxels. */
	using VoxelArray = NArray<INDEXING, VOXELSET_SIZE_T, DIMS...>;

private:
	/** The array were are stored the voxels. */
	VoxelArray _voxels;

	/** World size of a single voxel of this chunk. */
	float _voxelSize;
	float _halfVoxelSize;
	float _chunkWorldSize;
	float _halfChunkWorldSize;

	/** Position in the world */
	VectorNf<N> _worldPos;

	/**
	 * If chunk is free, it is not bound to the terrain's grid.
	 * Cool if you want to implement destructable terrain.
	 */
	bool _isFreeChunk = false;

	/** Depend from _isFreeChunk : */
	union {
		/** If is not free chunk : coordinate relative to terrain. */
		//Vector<size_t, N> _gridCoords;

		/** If is free chunk : transformation relative to terrain. */
		//Ra::Core::Transform _transform;
	};

public:

	/** Define world's size voxel. */
	Chunk(float worldSize)
	{
		_voxelSize = worldSize/OpPack::Proj(0, DIMS...);
		_halfVoxelSize = _voxelSize*0.5f;
		_chunkWorldSize = worldSize;
		_halfChunkWorldSize = _chunkWorldSize*0.5f;
	}

	/** Number of neightbor per voxel. */
	static constexpr size_t N_NEIGHBOR = N*2;

	/** List who store neighbor voxel. */
	using ListNeighbor = std::array<VOXELSET_SIZE_T, N_NEIGHBOR>;

	/** Get list of neightbor voxel (for any dimension). */
	ListNeighbor GetNeighborVoxels(const typename VoxelArray::Coordinates& coords) const
	{
		constexpr VOXELSET_SIZE_T defaultVoxelID = 0;
		ListNeighbor neighbors;
		for(int n = 0; n < N; ++n) {
			// Postive axis
			if(coords[n] >= OpPack::Proj(n, (DIMS)...)-1) {
				neighbors[n*2+0] = defaultVoxelID;
			} else {
				typename VoxelArray::Coordinates buf = coords;
				++buf[n];
				neighbors[n*2+0] = _voxels(buf);
			}
			//std::cout << (int)neighbors[n*2+0] << std::endl;

			// Negative axis
			if(coords[n] <= 0) {
				neighbors[n*2+1] = defaultVoxelID;
			} else {
				typename VoxelArray::Coordinates buf = coords;
				--buf[n];
				neighbors[n*2+1] = _voxels(buf);
			}
			//std::cout << (int)neighbors[n*2+1] << std::endl;

		}
		//std::cout << "####" << std::endl;

		return neighbors;
	}

	/** Get list of neightbor voxel (for any dimension). */
	/*ListNeighbor GetNeighborVoxels(const typename VoxelArray::Coordinates& coords) const
	{
		constexpr VOXELSET_SIZE_T defaultVoxelID = 1;
		ListNeighbor neighbors{defaultVoxelID,defaultVoxelID,defaultVoxelID,defaultVoxelID,defaultVoxelID,defaultVoxelID};

		E_AXIS axis = AXIS_X;
		if(coords[axis] < OpPack::Proj(axis, DIMS...)-1) {
			typename VoxelArray::Coordinates buf = coords;
			++buf[axis];
			neighbors[axis*2+0] = _voxels(buf);
		}
		if(coords[axis] > 0) {
			typename VoxelArray::Coordinates buf = coords;
			--buf[axis];
			neighbors[axis*2+1] = _voxels(buf);
		}

		axis = AXIS_Y;
		if(coords[axis] < OpPack::Proj(axis, DIMS...)-1) {
			typename VoxelArray::Coordinates buf = coords;
			++buf[axis];
			neighbors[axis*2+0] = _voxels(buf);
		}
		if(coords[axis] > 0) {
			typename VoxelArray::Coordinates buf = coords;
			--buf[axis];
			neighbors[axis*2+1] = _voxels(buf);
		}

		axis = AXIS_Z;
		if(coords[axis] < OpPack::Proj(axis, DIMS...)-1) {
			typename VoxelArray::Coordinates buf = coords;
			++buf[axis];
			neighbors[axis*2+0] = _voxels(buf);
		}
		if(coords[axis] > 0) {
			typename VoxelArray::Coordinates buf = coords;
			--buf[axis];
			neighbors[axis*2+1] = _voxels(buf);
		}

		return neighbors;
	}*/

	/** Get world pos of given voxel inside the chunk. */
	template<size_t I = 0>
	inline VectorNf<N> GetWorldPos(const typename VoxelArray::Coordinates& arrayCoords)
	{
		if constexpr (I == N)
			return VectorNf<N>();
		else {
			auto v = GetWorldPos<I+1>(arrayCoords);
			v[I] = arrayCoords[I]*_voxelSize - _halfChunkWorldSize + _halfVoxelSize + _worldPos[I];
			return v;
		}
	}

	/** Know if world coordinate is inside chunk. */
	inline bool isPointInsideChunk(const VectorNf<N>& worldPos)
	{
		//coords[0]*_voxelSize + _halfVoxelSize -_halfChunkWorldSize + _worldPos[0],
		for(size_t n = 0; n < N; ++n) {
			if(worldPos[n] >= (_halfChunkWorldSize-_halfVoxelSize) + _worldPos[n]) {
				return false;
			}
			if(worldPos[n] <= (-_halfChunkWorldSize-_halfVoxelSize) + _worldPos[n]) {
				return false;
			}
		}
		return true;
	}

	/** Get chunk coordinate at */
	template<size_t I = 0>
	inline typename VoxelArray::Coordinates GetChunkVoxelCoordinatesAt(const VectorNf<N>& worldPos)
	{
		if constexpr (I == N)
			return typename VoxelArray::Coordinates();
		else {
			auto v = GetChunkVoxelCoordinatesAt<I+1>(worldPos);
			v[I] = (worldPos[I] - _worldPos[I] + _halfChunkWorldSize - _halfVoxelSize)/_voxelSize;
			return v;
		}
	}

	/**
	 * Type function used given as parameter to 'Procedural'.
	 * Return new voxel id to assign in VoxelArray.
	 */
	using FunProcedural = VOXELSET_SIZE_T (*) (const Chunk& chunk, const VectorNf<N> worldPos, const typename VoxelArray::Coordinates& arrayCoords, const VOXELSET_SIZE_T previousVoxelID);
	
	/** Fill chunk with the id in parameter. */
	void Fill(VOXELSET_SIZE_T voxelID)
	{
		auto itBegin = _voxels.begin();
		for(auto it = _voxels.begin(); it != _voxels.end(); ++it) {
        	auto index = std::distance(itBegin, it);
        	_voxels[index] = voxelID;
    	}
	}

	/** Execute a function for each voxel. */
	template<typename F>
	void Procedural(F fun)
	{
		auto itBegin = _voxels.begin();
		//#pragma omp parallel for
		for(auto it = _voxels.begin(); it != _voxels.end(); ++it) {
        	auto index = std::distance(itBegin, it);

        	// Calculate array coords, and world position :
        	typename VoxelArray::Coordinates arrayCoords;
        	_voxels.CoordsFor(arrayCoords, index);
			VectorNf<N> worldPos = GetWorldPos(arrayCoords);
        	// Call client's function
        	auto newVoxelID = fun(
        		*this,
        		worldPos,
        		arrayCoords,
        		_voxels[index]
        	);
        	//ASSERT(newVoxelID < VoxelSet.GetSize(), "Assigned procedural voxel is not in the set.");
        	_voxels[index] = newVoxelID;
    	}
	}

	/**
	 * Generate a mesh composed of cubes from chunk.
	 */
	TriangleMesh CubicMesh(const VoxelSet<VOXELSET_SIZE_T>& voxelSet)
	{
		static_assert(N == 3, "A cubic mesh is only for a 3D space.");

		TriangleMesh mesh;
	    mesh.addAttrib("in_color", Ra::Core::Vector4Array{});

		auto itBegin = _voxels.begin();
		for(auto it = _voxels.begin(); it != _voxels.end(); ++it) {
			auto index = std::distance(itBegin, it);
        	auto coords = VoxelArray::CoordsFor(index);

        	// Calculate offset voxel, relative to this chunk.
        	Vector3f offset(
        		coords[0]*_voxelSize + _halfVoxelSize -_halfChunkWorldSize + _worldPos[0],
        		coords[1]*_voxelSize + _halfVoxelSize -_halfChunkWorldSize + _worldPos[1],
        		coords[2]*_voxelSize + _halfVoxelSize -_halfChunkWorldSize + _worldPos[2]
        	);

        	// Find what kind of voxel is stored here.
        	auto voxelID = _voxels[index];
        	const Voxel<VOXELSET_SIZE_T>& voxel = voxelSet.Get(voxelID);
			if(voxel.visible) {
				ListNeighbor neighbors = GetNeighborVoxels(coords);
				voxel.AppendCube(neighbors, offset, mesh, _halfVoxelSize);
			}
    	}
    	mesh.checkConsistency();

    	return mesh;
	}

	/**
	 * Get width of the array on a given axes at compile time.
	 */
	template<size_t AXIS>
	constexpr static inline size_t WidthOf()
	{
		static_assert(
			AXIS < N,
			"Demanding the size of an axis of higher dimension from this chunk."
		);
		return OpPack::Proj(AXIS, DIMS...);
	}

	constexpr static inline size_t GetWidth()	{ return WidthOf<AXIS_X>(); }
	constexpr static inline size_t GetHeight()	{ return WidthOf<AXIS_Y>(); }
	constexpr static inline size_t GetDepth()	{ return WidthOf<AXIS_Z>(); }
	constexpr static inline size_t GetSpissitude()	{ return WidthOf<AXIS_W>(); }

	/** Set voxel at world given position. */
	inline bool SetVoxelAt(const VectorNf<N>& worldPos, const VOXELSET_SIZE_T voxelID)
	{
		if(isPointInsideChunk(worldPos)) {
			SetVoxel(GetChunkVoxelCoordinatesAt(worldPos), voxelID);
			return true;
		} else return false;
	}

	/** Set voxel in chunk. */
	inline void SetVoxel(const typename VoxelArray::Coordinates coords, const VOXELSET_SIZE_T voxelID)
	{
		_voxels(coords) = voxelID;
	}


	/** Get voxel in chunk. */
	inline VOXELSET_SIZE_T GetVoxel(const typename VoxelArray::Coordinates coords)
	{
		return _voxels(coords);
	}

	/** Draw sphere at world position. */
	/*void DrawSphereAt(const VectorNf& worldPos, const float radius, const VOXELSET_SIZE_T voxelID)
	{
		std::array<size_t, N> sizes; for(size_t i = 0; i < N; ++i) sizes[i] = radius;
		NestedForLoops([this] (const std::array<size_t, N>& coords) {
			this->SetVoxelAt(worldPos, voxelID)
		},sizes);
	}*/

	/*void DrawCapsule(const VectorNf& worldA, const float radiusA, const VectorNf& worldB, const float radiusB, const VOXELSET_SIZE_T voxelID)
	{

	}*/

	/** Draw line from A to B. */
	void DrawLine(const VectorNf<N>& worldA, const VectorNf<N>& worldB, const float radius, const VOXELSET_SIZE_T voxelID)
	{
		ASSERT(radius >= 0.0f, "Line's radius cannot be negative.");
		for(float k = 0; k <= 1.0f; k += 0.01f) {
			SetVoxelAt(Math::lerp(worldA, worldB, k), voxelID);
		}
		/*auto dist = Math::enclidian_dist(worldA, worldB);
		auto vecAB = VecAToB(worldA, worldB)
		for(float i = 0; i < dist; i += _voxelSize) {

		}*/
	}

};

#include "Chunk.inl"

template<typename VOXELSET_SIZE_T = uint8>
using Chunk1 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 1, 1, 1>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk2 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 2, 2, 2>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk4 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 4, 4, 4>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk8 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 8, 8, 8>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk16 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 16, 16, 16>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk32 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 32, 32, 32>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk64 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 64, 64, 64>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk128 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 128, 128, 128>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk256 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 256, 256, 256>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk512 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 512, 512, 512>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk1024 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 1024, 1024, 1024>;

template<typename VOXELSET_SIZE_T = uint8>
using Chunk2048 = Chunk<IndexingMode::S_ORDERING, VOXELSET_SIZE_T, 2048, 2048, 2048>;


/** Unit test for Chunk class. */
void unitests_chunk();

} // namespace HyperV