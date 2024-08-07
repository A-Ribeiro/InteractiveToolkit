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
#include "../Thread.h"

#include "unamed_fake_sem.h"

namespace Platform
{

    static int fake_sem_init(fake_sem_t* psem, int flags, unsigned count) {
        fake_sem_t pnewsem;
        int result;
        result = pthread_mutex_init(&pnewsem.mutex, nullptr);
        if (result)
            return result;
        result = pthread_cond_init(&pnewsem.cond_var, nullptr);
        if (result) {
            pthread_mutex_destroy(&pnewsem.mutex);
            return result;
        }
        pnewsem.count = count;
        *psem = pnewsem;
        return 0;
    }

    static int fake_sem_destroy(fake_sem_t *psem) {
        if (!psem)
            return EINVAL;
        pthread_mutex_destroy(&psem->mutex);
        pthread_cond_destroy(&psem->cond_var);
        return 0;
    }

    static int fake_sem_post(fake_sem_t *pxsem) {
        if (!pxsem)
            return EINVAL;
        int result, xresult;
        result = pthread_mutex_lock(&pxsem->mutex);
        if (result)
            return result;
        pxsem->count = pxsem->count + 1;
        xresult = pthread_cond_signal(&pxsem->cond_var);
        result = pthread_mutex_unlock(&pxsem->mutex);
        if (result)
            return result;
        if (xresult) {
            errno = xresult;
            return -1;
        }
        return 0;
    }

    static int fake_sem_wait(fake_sem_t *pxsem, bool ignore_signal) {
        int result, xresult;
        if (!pxsem)
            return EINVAL;
        result = pthread_mutex_lock(&pxsem->mutex);
        if (result)
            return result;
        xresult = 0;
        while (
        ( ignore_signal ||
            !Platform::Thread::isCurrentThreadInterrupted()
        ) && pxsem->count == 0) {
            xresult = pthread_cond_wait(&pxsem->cond_var, &pxsem->mutex);
            if (xresult) // any error...
                break;
        }
        if (!ignore_signal && Platform::Thread::isCurrentThreadInterrupted())
            xresult = EINTR;
        if (!xresult && pxsem->count > 0)
            pxsem->count--;
        result = pthread_mutex_unlock(&pxsem->mutex);
        if (result)
            return result;
        if (xresult) {
            errno = xresult;
            return -1;
        }
        return 0;
    }

    static int fake_sem_trywait(fake_sem_t *pxsem) {
        int result, xresult;
        if (!pxsem)
            return EINVAL;
        result = pthread_mutex_lock(&pxsem->mutex);
        if (result)
            return result;
        xresult = 0;
        if (pxsem->count > 0)
            pxsem->count--;
        else
            xresult = EAGAIN;
        result = pthread_mutex_unlock(&pxsem->mutex);
        if (result)
            return result;
        if (xresult) {
            errno = xresult;
            return -1;
        }
        return 0;
    }

    static int fake_sem_timedwait(fake_sem_t *pxsem, const struct timespec *abstim, bool ignore_signal) {
        int result, xresult;
        if (!pxsem)
            return EINVAL;
        result = pthread_mutex_lock(&pxsem->mutex);
        if (result)
            return result;
        xresult = 0;
        while (
            (
                ignore_signal ||
                !Platform::Thread::isCurrentThreadInterrupted()
                ) && pxsem->count == 0) {
            xresult = pthread_cond_timedwait(&pxsem->cond_var, &pxsem->mutex, abstim);
            if (xresult) //ETIMEDOUT or any other error...
                break;
        }
        if (!ignore_signal && Platform::Thread::isCurrentThreadInterrupted())
            xresult = EINTR;
        if (!xresult && pxsem->count > 0)
            pxsem->count--;
        result = pthread_mutex_unlock(&pxsem->mutex);
        if (result)
            return result;
        if (xresult) {
            errno = xresult;
            return -1;
        }
        return 0;
    }


}
