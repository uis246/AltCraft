#pragma once

#include <stdint.h>

#if USE_OPTICK
#include <optick.h>
#else
#define OPTICK_EVENT(...)
#define OPTICK_CATEGORY(NAME, COLOR)
#define OPTICK_FRAME(NAME)
#define OPTICK_THREAD(FRAME_NAME)
#define OPTICK_START_THREAD(FRAME_NAME)
#define OPTICK_STOP_THREAD()
#define OPTICK_TAG(NAME, DATA)
#define OPTICK_EVENT_DYNAMIC(NAME)
#define OPTICK_PUSH_DYNAMIC(NAME)
#define OPTICK_PUSH(NAME)
#define OPTICK_POP()
#define OPTICK_CUSTOM_EVENT(DESCRIPTION)
#define OPTICK_STORAGE_REGISTER(STORAGE_NAME)
#define OPTICK_STORAGE_EVENT(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START, CPU_TIMESTAMP_FINISH)
#define OPTICK_STORAGE_PUSH(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START)
#define OPTICK_STORAGE_POP(STORAGE, CPU_TIMESTAMP_FINISH)
#define OPTICK_SET_STATE_CHANGED_CALLBACK(CALLBACK)
#endif

#define AC_THREAD_SET_NAME(name) OPTICK_THREAD(name)

#ifdef _WIN32
#	define AC_API __declspec(dllexport)
#	define AC_INTERNAL
#else
#	ifdef __GNUC__
#		define AC_API __attribute__((visibility("default")))
#		define AC_INTERNAL __attribute__((visibility("internal")))
#	else
#		define AC_API
#		define AC_INTERNAL
#	endif
#endif

#ifdef __linux__
#	include <pthread.h>
#	undef AC_THREAD_SET_NAME
#	define AC_THREAD_SET_NAME(name) OPTICK_THREAD(name);pthread_setname_np(pthread_self(), name)
#endif

#if USE_SDT
//TODO
#	include <sys/sdt.h>
#endif

#if defined(__M_IX86)||defined(__i386__)||defined(__amd64__)||defined(_M_AMD64)||defined(__arm__)
//Inversed negative integers
#define floorASR(value, shmat, dest) (dest=value >> shmat)
#define floorASRQ(value, shmat, dest) (dest=value >> shmat)
#else
#define floorASR(value, shmat, dest) (dest=std::floor(value / (float)(2 << shmat)))
#define floorASRQ(value, shmat, dest) (dest=std::floor(value / (float)(2 << shmat)))
#endif

#ifndef __GNUC__
#	ifndef WIN32
#		error This does not works
#		define mv64(x, i) (x&(0xFF<<(i*8)))>>((7-i)*8)
#		define bswap_64(x) ( mv64(x, 0) | mv64(x, 1) | mv64(x, 2) | mv64(x, 3) | mv64(x, 4) | mv64(x, 5) | mv64(x, 6) | mv64(x, 7))
#	else
#		define bswap_64(x) _byteswap_uint64(x)
#	endif
#else
#	include <byteswap.h>
#endif
