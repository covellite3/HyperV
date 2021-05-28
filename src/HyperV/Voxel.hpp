/**
 * \author Asso Corentin
 * \Date April 20 2021
 * \Desc Defintion of Voxel.
 */
#pragma once

#include <Core/Types.hpp>
#include <Core/Geometry/TriangleMesh.hpp>

#include "Util.hpp"
#include "VoxelSet.hpp"

namespace HyperV {

enum E_NEIGHBOR { POS_X=0, NEG_X=1, POS_Y=2, NEG_Y=3, POS_Z=4, NEG_Z=5 };

template<typename VOXELSET_SIZE_T>
class VoxelSet;


/**
 * Hyper voxel data structure.
 * - VOXELSET_SIZE_T is the kind of index used by
 * the voxelset, this voxel is part of.
 */
template<typename VOXELSET_SIZE_T>
class Voxel {
	friend VoxelSet<VOXELSET_SIZE_T>;

private:
	VoxelSet<VOXELSET_SIZE_T>* _voxelset = NULL;

public:

	/** Name of the voxel. */
	std::string name;

	/** Is visible ? Then will be discard at rendering. Perfect for Air voxel. */
	bool visible;

	/** Color of the voxel. */
	Colorf color;

	/** Volume of the note play for sonification, 0 for mute, 1 for loud. */
	float sonification;

	Voxel() {}

	Voxel(const std::string& name, bool visible, const Ra::Core::Utils::Colorf& color, float sonification)
	: name(name), visible(visible), color(color), sonification(sonification) {}


	/** Add single square representing the 2D hyper-voxel to a mesh. */
	void AppendSquare(
		const std::array<VOXELSET_SIZE_T, 4> neighbors,
		const Vector3f& offset,
		TriangleMesh& mesh,
		const Vector3f& halfVoxelSizeVec
	) const;

	/** Add single cube representing the 3D hyper-voxel to a mesh. */
	void AppendCube(
		const std::array<VOXELSET_SIZE_T, 6> neighbors,
		const Vector3f& offset,
		TriangleMesh& mesh,
		const float halfVoxelSize
	) const;

	/** Say if a shared face between this voxel and another is visible. */
	bool isFaceVisible(VOXELSET_SIZE_T id) const;
};
#include "Voxel.inl"

} // namespace HyperV