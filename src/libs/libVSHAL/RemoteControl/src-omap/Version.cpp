#include "Version.h"


using namespace LM_VSHAL;


int Version::versionMajor()
{
    return VERSION_MAJOR;
}


int Version::versionMinor()
{
    return VERSION_MINOR;
}


Version::Architecture_t Version::architecture()
{
    return VARIANT_OMAP;
}


std::string Version::architectureDesc()
{
    return "OMAP";
}


Version::System_t Version::system()
{
    return SYSTEM_RC;
}


std::string Version::systemDesc()
{
    return "RemoteControl";
}
