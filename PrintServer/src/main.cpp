#include "Core.h"

extern "C" void app_main() 
{
    PrintServer::Core core;
    
    while (core.is_running())
    {
        core.update();
    }
}