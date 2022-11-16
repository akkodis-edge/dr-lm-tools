#include "Lock.h"


using namespace LM_VSHAL;


Lock::Lock(pthread_mutex_t &a_Mutex)
: m_Mutex(a_Mutex)
{
}

Lock::~Lock()
{
}


void Lock::lock()
{
    pthread_mutex_lock(&m_Mutex);
}


void Lock::unlock()
{
    pthread_mutex_unlock(&m_Mutex);
}
