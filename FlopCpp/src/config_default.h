/* include the COIN-OR-wide system specific configure header */
#include "configall_system.h"

/* this needs to come before the include of config_flopcpp_default.h */
#ifndef FLOPCPPLIB_EXPORT
#if defined(_WIN32) && defined(DLL_EXPORT)
#define FLOPCPPLIB_EXPORT __declspec(dllexport)
#else
#define FLOPCPPLIB_EXPORT
#endif
#endif

/* include the public project specific macros */
#include "config_flopcpp_default.h"
