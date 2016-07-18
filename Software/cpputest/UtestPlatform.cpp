/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "CppUTest/TestHarness.h"
#undef malloc
#undef free
#undef calloc
#undef realloc

#include "CppUTest/PlatformSpecificFunctions.h"

static jmp_buf test_exit_jmp_buf[10];
static int jmp_buf_index = 0;

extern "C" int PlatformSpecificSetJmpImplementation(void (*function) (void* data), void* data)
{
    if (0 == setjmp(test_exit_jmp_buf[jmp_buf_index])) {
        jmp_buf_index++;
        function(data);
        jmp_buf_index--;
        return 1;
    }
    return 0;
}

static void PlatformSpecificLongJmpImplementation()
{
    jmp_buf_index--;
    longjmp(test_exit_jmp_buf[jmp_buf_index], 1);
}

static void PlatformSpecificRestoreJumpBufferImplementation()
{
    jmp_buf_index--;
}

void (*PlatformSpecificLongJmp)() = PlatformSpecificLongJmpImplementation;
int (*PlatformSpecificSetJmp)(void (*)(void*), void*) = PlatformSpecificSetJmpImplementation;
void (*PlatformSpecificRestoreJumpBuffer)() = PlatformSpecificRestoreJumpBufferImplementation;

void RunTestInASeperateProcess(UtestShell* shell, TestPlugin* plugin, TestResult* result)
{
   printf("-p doesn't work on this platform as it is not implemented. Running inside the process\b");
   shell->runOneTest(plugin, *result);
}

void (*PlatformSpecificRunTestInASeperateProcess)(UtestShell*, TestPlugin*, TestResult*) = &RunTestInASeperateProcess;
int (*PlatformSpecificFork)() = NULL;
int (*PlatformSpecificWaitPid)(int, int*, int) = NULL;

TestOutput::WorkingEnvironment PlatformSpecificGetWorkingEnvironment()
{
    return TestOutput::eclipse;
}

extern long getMilliseconds();

long (*GetPlatformSpecificTimeInMillis)() = &getMilliseconds;

static const char* TimeStringImplementation()
{
    //time_t tm = time(NULL);
    return "";//ctime(&tm);
}

const char* (*GetPlatformSpecificTimeString)() = TimeStringImplementation;

int (*PlatformSpecificVSNprintf)(char *str, size_t size, const char* format, va_list args) = &vsnprintf;

/* IO operations */
PlatformSpecificFile FOpen(const char* filename, const char* flag)
{
	(void)filename;
	(void)flag;
	return 0;
}
void FPuts(const char* str, PlatformSpecificFile file)
{
	(void)str;
	(void)file;
}
void FClose(PlatformSpecificFile file)
{
	(void)file;
}

PlatformSpecificFile (*PlatformSpecificFOpen)(const char* filename, const char* flag) = &FOpen;
void (*PlatformSpecificFPuts)(const char* str, PlatformSpecificFile file) = &FPuts;
void (*PlatformSpecificFClose)(PlatformSpecificFile file) = &FClose;

void Flush(void){}

int (*PlatformSpecificPutchar)(int c) = &putchar;
void (*PlatformSpecificFlush)(void) = &Flush;

/* Dynamic Memory operations */
void* (*PlatformSpecificMalloc)(size_t) = &malloc;
void* (*PlatformSpecificRealloc)(void*, size_t) = &realloc;
void (*PlatformSpecificFree)(void*) = &free;
void* (*PlatformSpecificMemCpy)(void*, const void*, size_t) = &memcpy;
void* (*PlatformSpecificMemset)(void*, int, size_t) = &memset;

static int IsNanImplementation(double d)
{
    return isnan(d);
}

int (*PlatformSpecificIsNan)(double) = IsNanImplementation;
double (*PlatformSpecificFabs)(double) = fabs;

void NotSupported(PlatformSpecificMutex mtx)
{
	(void)mtx;
//	assert(false);
}

PlatformSpecificMutex NotSupported()
{
//	assert(false);
	return 0;
}

PlatformSpecificMutex (*PlatformSpecificMutexCreate)(void) = &NotSupported;
void (*PlatformSpecificMutexLock)(PlatformSpecificMutex mtx) = &NotSupported;
void (*PlatformSpecificMutexUnlock)(PlatformSpecificMutex mtx) = &NotSupported;
void (*PlatformSpecificMutexDestroy)(PlatformSpecificMutex mtx) = &NotSupported;

