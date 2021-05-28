#include "HyperVWindow.hpp"

#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

// include the Engine/entity/component interface
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>

#include <Gui/Utils/KeyMappingManager.hpp>

namespace HyperV {

/** Defining game's types. */
using IndexVoxelSet = uint8;
using GameVoxelSet = VoxelSet<IndexVoxelSet>;
using GameChunk = Chunk32<IndexVoxelSet>;

/** Game's object. */
GameChunk chunk(16);
GameVoxelSet voxelSet = GameVoxelSet::GenDefaultSet();
Vector3f offset(0.0f, 0.0f, 0.0f);

/** Will generate default terrain. */
IndexVoxelSet DefaultTerrainGen(
    const GameChunk& chunk,
    const Ra::Core::Vector3f worldPos,
    const typename GameChunk::VoxelArray::Coordinates& coords,
    const IndexVoxelSet previousVoxelID)
{
    const float seed = 7;
    const float threshold = 0.75f;
    const float scale = 0.0625f*3;
    const float levelMin = 4;
    const float levelMax = 6;
    const float topography = levelMax-levelMin;

    if(worldPos[1] < levelMax) {
        // Bellow highest mountaines
        float perlin2D = Procedural::PerlinNoise<2>(Vector2f(worldPos[0], worldPos[2]), Vector2f(scale, scale), seed);
        float groundLevel = perlin2D*topography+levelMin;
        if(worldPos[1] > groundLevel) {
            // Above ground
            return 0;
        } else {
            // Bellow ground
            float perlin3D = Procedural::PerlinNoise<3>(worldPos, Vector3f(scale, scale, scale), seed);
            if(perlin3D < threshold) {
                // Solid ground
                float distSurface = groundLevel-worldPos[1];
                if(distSurface < 1) return 1; // Grass
                else if(distSurface < 4) return 2; // Dirt
                else if(worldPos[1] < -7) return 5; // Bedrock
                else {
                    // Deep bellow ground
                    if(threshold-perlin3D < 0.15f) {
                        // Cave wall
                        return 4; // Cobble
                    } else {
                        // Deep underground
                        return 3; // Stone
                    }
                }
            } else {
                // empty cave
                return 0; // Air
            }
        }
    } else {
        // Above highest mountaines
        return 0; // Air
    }
}

/** Generate tree at given coordinate. */
void GenTreeAt(
    GameChunk& chunk,
    const Ra::Core::Vector3f worldPos,
    const float radiusTree,
    const float segmentLenght,
    const IndexVoxelSet voxelLogID,
    const IndexVoxelSet voxelLeafID,
    const size_t depth = 0)
{
    const float seed = 7;
    constexpr size_t N = 3; // DIM

    // Max recursion
    if(depth >= 6) return;

    // Random vector who give direction of tree branch
    auto randomVec = Procedural::RNG_VEC<N>(worldPos, seed);
    float lenghtRandomVec = Math::lenght<N>(randomVec);
    if(lenghtRandomVec > 0)
    	randomVec = Math::normalize<N>(randomVec, 1/lenghtRandomVec);
    else {
    	//std::cout << "missed" << std::endl;
    	randomVec = VectorNf<N>();
    	lenghtRandomVec = 1.0f;
    	randomVec[0] = lenghtRandomVec;
    }

    // Position of the last point of the tree branch
    //auto endSegment = worldPos + randomVec*segmentLenght;
    auto endSegment = Math::Add<N>(worldPos, Math::MulScalar<N>(randomVec, segmentLenght));

    // Draw segment
   	chunk.DrawLine(worldPos, endSegment, radiusTree, voxelLeafID);

    // Draw other branchs recursively
    GenTreeAt(chunk, endSegment, radiusTree, segmentLenght, voxelLogID, voxelLeafID, depth+1);
}

HyperVWindow::HyperVWindow( uint w, uint h, QWidget* parent ) : Ra::Gui::SimpleWindow( w, h, parent )
{
    setWindowTitle( QString( "HyperV" ) );

	auto engine = Ra::Engine::RadiumEngine::getInstance();
	auto geometrySystem = engine->getSystem( "GeometrySystem" );

	Ra::Gui::KeyMappingManager::getInstance()->loadConfiguration("/home/asso/cpp-workspace/HyperV/default.xml");

	// Generate terrain
	//chunk.DrawLine(Vector3f(), Vector3f(0.0f, 16.0f, 0.0f), 1, 3);
	chunk.Procedural(DefaultTerrainGen);
	/*GenTreeAt(
		chunk,
		Vector3f(0.0f, 0.0f, 0.0f),
		1.0f, // radiusTree
		1.0f, // segmentLenght
		7, // Tree's log id
		6  // Tree's leaf id
	);*/
    // Generate chunk's entity and model
    TriangleMesh chunkMesh = chunk.CubicMesh(voxelSet);
    auto e = engine->getEntityManager()->createEntity("Chunk");
    auto c = new Ra::Engine::Scene::TriangleMeshComponent("Chunk Mesh", e, std::move(chunkMesh), nullptr);
    geometrySystem->addComponent(e, c);


    // Setting up game loop :
    auto close_timer = new QTimer();
    close_timer->setInterval(500);
    QObject::connect(close_timer, &QTimer::timeout, [&chunk](){
    	// Move chunk offset
    	offset += Vector3f(0.1f, 0.0f, 0.1f);
    	//std::cout << offset[0] << ":" << offset[1] << ":" << offset[2] << std::endl;

    	// Generate terrain for chunk
    	//chunk.Procedural(DefaultTerrainGen);

    	// Recreate mesh for chunk
    });
    close_timer->start();
}

HyperVWindow::~HyperVWindow() = default;


void HyperVWindow::updateUi( Ra::Plugins::RadiumPluginInterface* )
{
    // no ui in the simple window, so, nothing to do
    //std::cout << "updateUi" << std::endl;
}

} // namespace HyperV
