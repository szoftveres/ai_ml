#ifndef __SERVICEAPI_H__
#define __SERVICEAPI_H__

typedef struct serviceapi_s {
    int (*pull_data) (char* buf, int len);
    int (*push_data) (char* buf, int len);
} serviceapi_t;


typedef int (*servicefn_t) (serviceapi_t*);


#endif /* __SERVICEAPI_H__ */
