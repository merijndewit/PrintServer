#include "Core.h"

namespace PrintServer
{
    Core::Core() : connection_successful(Wifi::init()), webserver()
    {

    }
}