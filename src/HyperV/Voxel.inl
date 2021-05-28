template<typename VOXELSET_SIZE_T>
void Voxel<VOXELSET_SIZE_T>::AppendCube(
	const std::array<VOXELSET_SIZE_T, 6> neighbors,
	const Vector3f& offset,
	TriangleMesh& mesh,
	const float halfVoxelSize
) const
{
	TriangleMesh cube;

	Vector3f
		a = Math::MulScalar<3>(Vector3f(-1, -1, -1), halfVoxelSize) + offset,
		b = Math::MulScalar<3>(Vector3f( 1, -1, -1), halfVoxelSize) + offset,
		c = Math::MulScalar<3>(Vector3f( 1, -1,  1), halfVoxelSize) + offset,
		d = Math::MulScalar<3>(Vector3f(-1, -1,  1), halfVoxelSize) + offset,

		e = Math::MulScalar<3>(Vector3f(-1,  1, -1), halfVoxelSize) + offset,
		f = Math::MulScalar<3>(Vector3f( 1,  1, -1), halfVoxelSize) + offset,
		g = Math::MulScalar<3>(Vector3f( 1,  1,  1), halfVoxelSize) + offset,
		h = Math::MulScalar<3>(Vector3f(-1,  1,  1), halfVoxelSize) + offset,

		UnitX = Vector3f(1, 0, 0),
		UnitY = Vector3f(0, 1, 0),
		UnitZ = Vector3f(0, 0, 1);

	//Add leftface +X
	if(isFaceVisible(neighbors[POS_X])) {
		TriangleMesh face;
		Vector3f normal = UnitX;
		face.setVertices({ b, c, f, g });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(3, 1, 0), Vector3ui(2, 3, 0) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face POX_X to the mesh.");
	}

	//Add rightface -X
	if(isFaceVisible(neighbors[NEG_X])) {
		TriangleMesh face;
		Vector3f normal = -UnitX;
		face.setVertices({ a, d, e, h });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(0, 1, 3), Vector3ui(0, 3, 2) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face NEG_X to the mesh.");
	}

	//Add upface +Y
	if(isFaceVisible(neighbors[POS_Y])) {
		TriangleMesh face;
		Vector3f normal = UnitY;
		face.setVertices({ e, f, g, h });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(3, 1, 0), Vector3ui(3, 2, 1) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face POS_Y to the mesh.");
	}

	//Add downface -Y
	if(isFaceVisible(neighbors[NEG_Y])) {
		TriangleMesh face;
		Vector3f normal = -UnitY;
		face.setVertices({ a, b, c, d });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(0, 1, 3), Vector3ui(1, 2, 3) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face NEG_Z to the mesh.");
	} 

	//Add backface +Z
	if(isFaceVisible(neighbors[POS_Z])) {
		TriangleMesh face;
		Vector3f normal = UnitZ;
		face.setVertices({ c, d, g, h });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(3, 1, 0), Vector3ui(2, 3, 0) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face POS_Z to the mesh.");
	}

	//Add frontface -Z
	if(isFaceVisible(neighbors[NEG_Z])) {
		TriangleMesh face;
		Vector3f normal = -UnitZ;
		face.setVertices({ a, b, e, f });
		face.setNormals({ normal, normal, normal, normal });
		face.setIndices({ Vector3ui(3, 1, 0), Vector3ui(2, 3, 0) });
		bool success = cube.append(face);
		ASSERT(success, "Unable to add face NEG_Z to the mesh.");
	}

	cube.addAttrib(
    	"in_color",
    	Ra::Core::Vector4Array {cube.vertices().size(), color}
    );

	cube.checkConsistency();

	bool success = mesh.append(cube);
	ASSERT(success, "Append cube to mesh error.");
}

template<typename VOXELSET_SIZE_T>
bool Voxel<VOXELSET_SIZE_T>::isFaceVisible(VOXELSET_SIZE_T id) const
{
	assert(_voxelset != NULL);
	// If this voxel is not visible, neither do his faces :
	/*if(!this->visible) return false;
	else {
		// If is neighbor is invisible, then his face is visible.
		return !_voxelset->Get(id).visible;
	}*/
	//return id == 0;
	return true;
}


