#ifndef __HOSTAPI_H__
#define __HOSTAPI_H__

#include <stdlib.h>
#include <stdarg.h>

typedef struct hostapi_s {
    int (*pull_data) (char* buf, int len);
    int (*push_data) (char* buf, int len);
    void* (*malloc)(size_t size);
    void (*free)(void *ptr);
    int (*conlog)(const char *format, ...);
} hostapi_t;


typedef int (*servicefn_t) (hostapi_t*);


#endif /* __HOSTAPI_H__ */
