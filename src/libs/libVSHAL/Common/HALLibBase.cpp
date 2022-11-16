#include "HALLibBase.h"


using namespace LM_VSHAL;


HALLibBase::HALLibBase()
{
    m_Version = new Version;
}


HALLibBase::~HALLibBase()
{
    delete m_Version;
}


Version &HALLibBase::getVersion()
{
    return *m_Version;
}


ConfigParams &HALLibBase::getConfigParams()
{
    return *m_ConfigParams;
}
