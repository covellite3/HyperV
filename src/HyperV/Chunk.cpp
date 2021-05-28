#include "Chunk.hpp"

void HyperV::unitests_chunk()
{
	Chunk4<uint8> chunk(16);
	// Empty chunk
	chunk.Fill(0);
	auto neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 0, "Neighbor POS_X should be empty.");
	ASSERT(neighbors[NEG_X] == 0, "Neighbor NEG_X should be empty.");
	ASSERT(neighbors[POS_Y] == 0, "Neighbor POS_Y should be empty.");
	ASSERT(neighbors[NEG_Y] == 0, "Neighbor NEG_Y should be empty.");
	ASSERT(neighbors[POS_Z] == 0, "Neighbor POS_Z should be empty.");
	ASSERT(neighbors[NEG_Z] == 0, "Neighbor NEG_Z should be empty.");

	// Empty chunk
	chunk.Fill(1);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// Testing each individual axis

	// POS_X
	chunk.Fill(1);
	chunk.SetVoxel({3, 2, 2}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 0, "Neighbor POS_X should be empty.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// NEG_X
	chunk.Fill(1);
	chunk.SetVoxel({1, 2, 2}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 0, "Neighbor NEG_X should be empty.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");
	
	// POS_Y
	chunk.Fill(1);
	chunk.SetVoxel({2, 3, 2}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 0, "Neighbor POS_Y should be empty.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// NEG_Y
	chunk.Fill(1);
	chunk.SetVoxel({2, 1, 2}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 0, "Neighbor NEG_Y should be empty.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// POS_Z
	chunk.Fill(1);
	chunk.SetVoxel({2, 2, 3}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 0, "Neighbor POS_Z should be empty.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// NEG_Z
	chunk.Fill(1);
	chunk.SetVoxel({2, 2, 1}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 0, "Neighbor NEG_Z should be empty.");

	// Testing each corner axis
	chunk.Fill(1);
	chunk.SetVoxel({1, 1, 1}, 0);
	chunk.SetVoxel({3, 1, 1}, 0);
	chunk.SetVoxel({1, 3, 1}, 0);
	chunk.SetVoxel({3, 3, 1}, 0);
	chunk.SetVoxel({1, 1, 3}, 0);
	chunk.SetVoxel({3, 1, 3}, 0);
	chunk.SetVoxel({1, 3, 3}, 0);
	chunk.SetVoxel({3, 3, 3}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");

	// Testing center
	chunk.Fill(1);
	chunk.SetVoxel({2, 2, 2}, 0);
	neighbors = chunk.GetNeighborVoxels({2, 2, 2});
	ASSERT(neighbors[POS_X] == 1, "Neighbor POS_X should be full.");
	ASSERT(neighbors[NEG_X] == 1, "Neighbor NEG_X should be full.");
	ASSERT(neighbors[POS_Y] == 1, "Neighbor POS_Y should be full.");
	ASSERT(neighbors[NEG_Y] == 1, "Neighbor NEG_Y should be full.");
	ASSERT(neighbors[POS_Z] == 1, "Neighbor POS_Z should be full.");
	ASSERT(neighbors[NEG_Z] == 1, "Neighbor NEG_Z should be full.");
}