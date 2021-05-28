/**
 * \author Asso Corentin
 * \Date April 29 2021
 * \Desc Defintion of VoxelSet.
 */

#pragma once

#include <Gui/BaseApplication.hpp>
#include <Gui/RaGui.hpp>

#include "Util.hpp"
#include "HyperVWindow.hpp"

namespace HyperV {

/**
 * Factory for HyperV's window.
 * This factory is to be used when instanciating a Ra::Gui::BaseApplication to serve
 * as display controler.
 */
class HyperVWindowFactory : public Ra::Gui::BaseApplication::WindowFactory
{
  public:
    using Ra::Gui::BaseApplication::WindowFactory::WindowFactory;
    inline Ra::Gui::MainWindowInterface* createMainWindow() const override {
        return new HyperVWindow();
    }
};

} // namespace HyperV
