#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include "HyperVWindowFactory.hpp"

using namespace HyperV;

/**
 * NOTE : Run cmd is :
 * MESA_GL_VERSION_OVERRIDE=4.5 MESA_GLSL_VERSION_OVERRIDE=450 ./src/HyperV/HyperV
 */

int main(int argc, char *argv[])
{
	HyperV::unitests_chunk();

    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( HyperV::HyperVWindowFactory {} );

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]
    
    return app.exec();
}
