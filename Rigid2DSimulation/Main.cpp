#include "SimulationApp.h"

#include <exception>

int main()
{
    try
    {
        SimulationApp().Run();
    }
    catch (std::exception const& error)
    {
        MessageBoxA(nullptr, error.what(), "Rigid2DSimulation", MB_OK | MB_ICONERROR);
        return 1;
    }
}
