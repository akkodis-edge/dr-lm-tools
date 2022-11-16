#ifndef LOCK_H_
#define LOCK_H_


#include <pthread.h>


namespace LM_VSHAL
{

//! \brief Class used for locking to make library thread safe
class Lock
{
public:
    //! \brief Constructor
    //! \param a_Mutex: Reference to mutex structure
    Lock(pthread_mutex_t &a_Mutex);
    //! \brief Destructor
    virtual ~Lock();

    //! \brief Locks resource
    void lock();
    //! \brief Unlocks resource
    void unlock();

private:
    pthread_mutex_t &m_Mutex;
};

}


#endif // LOCK_H_
