/**
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, undersquire
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SHL_THREAD.H
#define SHL_THREAD.H

#ifdef __cplusplus
extern "C" {
#endif

typedef void * shl_thread_t;
typedef void * shl_mutex_t;

/* Creates a running thread and returns its handle */
shl_thread_t shl_thread_create(void *(*func)(void *), void *arg);

/* Waits until specified thread terminates */
void *shl_thread_join(shl_thread_t thread);

/* Exits current thread */
void shl_thread_exit(void *data);

/* Creates a mutex */
shl_mutex_t shl_mutex_create(void);

/* Destroys a mutex */
int shl_mutex_destroy(shl_mutex_t mutex);

/* Locks a mutex */
int shl_mutex_lock(shl_mutex_t mutex);

/* Unlocks a mutex */
int shl_mutex_unlock(shl_mutex_t mutex);

#ifdef SHL_THREAD_IMPL

#ifdef _WIN32

#include <windows.h>

thread_t shl_thread_create(void *(*func)(void *), void *arg)
{
	return (shl_thread_t)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
}

void *shl_thread_join(shl_thread_t thread)
{
	void *data = (void *)WaitForSingleObject((HANDLE)thread, INFINITE);
	CloseHandle((HANDLE)thread);

	return data;
}

void shl_thread_exit(void *data)
{
	ExitThread((DWORD)data);
}

mutex_t shl_mutex_create(void)
{
	return (shl_mutex_t)CreateMutex(NULL, FALSE, NULL);
}

int shl_mutex_destroy(shl_mutex_t mutex)
{
	return CloseHandle((HANDLE)mutex);
}

int shl_mutex_lock(shl_mutex_t mutex)
{
	return (int)WaitForSingleObject((HANDLE)mutex, INFINITE);
}

int shl_mutex_unlock(shl_mutex_t mutex)
{
	return (int)ReleaseMutex((HANDLE)mutex);
}

#else

#include <stdlib.h>
#include <pthread.h>

thread_t shl_thread_create(void *(*func)(void *), void *arg)
{
	pthread_t thread;
	pthread_reate(&thread, NULL, func, arg);

	return (shl_thread_t)thread;
}

void *shl_thread_join(shl_thread_t thread)
{
	void *data;
	pthread_join((pthread_t)thread, &data);

	return data;
}

void shl_thread_exit(void *data)
{
	pthread_exit(data);
}

mutex_t shl_mutex_create(void)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);

	return (shl_mutex_t)mutex;
}

int shl_mutex_destroy(shl_mutex_t mutex)
{
	int result = pthread_mutex_destroy((pthread_mutex_t *)mutex);
	free(mutex);

	return result;
}

int shl_mutex_lock(shl_mutex_t mutex)
{
	return pthread_mutex_lock((pthread_mutex_t *)mutex);
}

int shl_mutex_unlock(shl_mutex_t mutex)
{
	return pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

#endif /* UNIX */

#endif /* IMPL */

#ifdef __cplusplus
}
#endif

#endif /* SHL_THREAD.H */
