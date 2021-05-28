/**
 * \author Asso Corentin
 * \Date April 29 2021
 * \Desc Defintion of VoxelSet.
 */

#pragma once

#include "Util.hpp"
#include <Gui/RaGui.hpp>
#include <Gui/RadiumWindow/SimpleWindow.hpp>

#include "Chunk.hpp"
#include "Procedural.hpp"

namespace HyperV {

class HyperVWindow : public Ra::Gui::SimpleWindow {

public:
	explicit HyperVWindow( uint w = 800, uint h = 640, QWidget* parent = nullptr );
	~HyperVWindow() override;

	/// Update the ui from the plugins loaded.
	void updateUi( Ra::Plugins::RadiumPluginInterface* plugin ) override;

};

} // namespace HyperV