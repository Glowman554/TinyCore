#pragma once

#ifdef DEBUG
void debugf(const char* fmt, ...);
#else
#define debugf(...)
#endif