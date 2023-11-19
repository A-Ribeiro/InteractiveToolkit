#pragma once

#if !defined(__APPLE__)
#error ERROR: Including Apple Unamed Semaphore in a Non-Apple OS
#endif

// for unamed semaphores on mac
//#include <dispatch/dispatch.h>
//dispatch_semaphore_t semaphore
//semaphore = dispatch_semaphore_create(count);
//dispatch_release(semaphore);
//return dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, 0)) == 0;
//uint64_t timeout = NSEC_PER_MSEC * timeout_ms;
//long s = dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, timeout));
//signaled = signaled || (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER) != 0);
//dispatch_semaphore_signal(semaphore);

#include "../platform_common.h"
#include "../Mutex.h"
//#include "../Thread.h"

namespace Platform
{

    // based on: https://stackoverflow.com/questions/641126/posix-semaphores-on-mac-os-x-sem-timedwait-alternative

    typedef struct
    {
        pthread_mutex_t mutex;
        pthread_cond_t cond_var;
        unsigned count;
    } fake_sem_t;

    static int fake_sem_init(fake_sem_t* psem, int flags, unsigned count);

    static int fake_sem_destroy(fake_sem_t *psem);

    static int fake_sem_post(fake_sem_t *pxsem) ;

    static int fake_sem_wait(fake_sem_t *pxsem, bool ignore_signal = false) ;

    static int fake_sem_trywait(fake_sem_t *pxsem) ;

    static int fake_sem_timedwait(fake_sem_t *pxsem, const struct timespec *abstim, bool ignore_signal = false) ;


}
