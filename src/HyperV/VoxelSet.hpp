/**
 * \author Asso Corentin
 * \Date April 20 2021
 * \Desc Defintion of VoxelSet.
 */
#pragma once

#include "Util.hpp"
#include "Array.hpp"
#include "Voxel.hpp"

#include <limits>

namespace HyperV {

template<typename SIZE_T> 
class Voxel;

class JsonFile;

/**
 * Store defintion of each voxel.
 * You can query a voxel with it's ID in the set.
 * VoxelSet is just a flat list. The number maximum of voxel,
 * it can hold depend of the type used for representing the index.
 * If SIZE_T type is uint8 then it can store 2^8=256 definitions of voxel.
 * SIZE_T must be an unsigned integer.
 */
template<typename SIZE_T> 
class VoxelSet {
public:
	static constexpr size_t MAX_SIZE = ((size_t)0b1)<<(8*sizeof(SIZE_T));

private:
	std::array<Voxel<SIZE_T>, MAX_SIZE> _voxels;
	size_t _size = 0;

public:

	/** Generate a default voxelset. */
	static VoxelSet GenDefaultSet()
	{
		VoxelSet set;

	    set.Append(Voxel<SIZE_T>( // 0
	        "Air", false, Colorf(1.0f, 1.0f, 1.0f), 0.0f
	    ));
	    set.Append(Voxel<SIZE_T>( // 1
	        "Grass", true, Colorf(0.0f, 1.0f, 0.0f), 1.0f
	    ));
	    set.Append(Voxel<SIZE_T>( // 2
	        "Dirt", true, Colorf(1.0f, 0.25f, 0.25f), 0.75f
	    ));
	    set.Append(Voxel<SIZE_T>( // 3
	        "Stone", true, Colorf(0.5f, 0.5f, 0.5f), 0.25f
	    ));
	    set.Append(Voxel<SIZE_T>( // 4
	        "Cobble Stone", true, Colorf(0.25f, 0.25f, 0.25f), 0.50f
	    ));
	    set.Append(Voxel<SIZE_T>( // 5
	        "Bedrock", true, Colorf(0.1f, 0.1f, 0.1f), 0.90f
	    ));
	    set.Append(Voxel<SIZE_T>( // 6
	        "Oak Leaf", true, Colorf(0.25f, 0.85f, 0.25f), 0.25f
	    ));
	    set.Append(Voxel<SIZE_T>( // 7
	        "Oak Log", true, Colorf(0.75f, 0.25f, 0.25f), 0.95f
	    ));

		return set;
	}

	/** Generate a voxelset representing gray scale. */
	static VoxelSet GenGrayScaleSet()
	{
		VoxelSet set;
		SIZE_T i = 0;
		while(true) // The devil's loop :p
		{
			float grayness = ((float)i)/std::numeric_limits<SIZE_T>::max();
			set.Append(Voxel("", true, {grayness, grayness, grayness}, grayness));
			// We cannot put the condition in the while predicat
			// because it would cause integer overflow and we would loop
			// forever.
			if(i == std::numeric_limits<SIZE_T>::max()) break;
			i++;
		}
		return set;
	}

	/** Generate a voxelset representing who visible after threshold. */
	static VoxelSet GenVisibleOn(float threshold, const Ra::Core::Utils::Colorf& color)
	{
		VoxelSet set;
		SIZE_T i = 0;
		while(true) // The devil's loop :p
		{
			float v = ((float)i)/std::numeric_limits<SIZE_T>::max();
			set.Append(Voxel("", (v > threshold), color, (v > threshold) ? 1 : 0));

			// We cannot put the condition in the while predicat
			// because it would cause integer overflow and we would loop
			// forever.
			if(i == std::numeric_limits<SIZE_T>::max()) break;
			i++;
		}
		return set;
	}

	/** Load voxel set from Json file. */
	static bool FromJson(JsonFile& json, VoxelSet& set);

	/** Store voxel as an Json file. */
	static bool ToJson(JsonFile& json, const VoxelSet& set);

	/** Get voxel for given id. */
	inline const Voxel<SIZE_T>& Get(SIZE_T id) const
	{
		ASSERT(id < _size, "No such id in voxelset.");
		return _voxels[id];
	}

	/** Copy voxel in set for given id. */
	inline void Set(SIZE_T id, const Voxel<SIZE_T>& voxel)
	{
		ASSERT(id < _size, "Out of bound id in voxelset.");
		_voxels[id] = voxel;
		_voxels[id]._voxelset = this;
	}

	/** Append voxel to the set. */
	inline void Append(const Voxel<SIZE_T>& voxel)
	{
		ASSERT(_size < MAX_SIZE, "Maximum number of voxel in set reached.");
		Set(_size++, voxel);
	}

	/** Get number of voxel stored in the set. */
	inline SIZE_T GetSize() { return _size; }

	static_assert(
		std::is_unsigned<SIZE_T>(),
		"VoxelSet must use unsigned integer for it's index."
	);

	static_assert(
		sizeof(SIZE_T) < sizeof(size_t),
		"We cannot store MAX_SIZE value for SIZE_T equal or higher to size_t."
	);
};

using VoxelSet8 = VoxelSet<uint8>;
using VoxelSet16 = VoxelSet<uint16>;
using VoxelSet32 = VoxelSet<uint32>;

} // namespace HyperV