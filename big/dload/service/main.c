#include "../hostapi.h"

int increment (int a);

hostapi_t* host;



/* By convention the entry point is the first function */

int _start (hostapi_t* h) {
    int b;
    int* p;

    host = h;

    host->conlog("Service dynamically linked and running\n");

    host->pull_data((char*)&b, sizeof(b));

    p = (int*) host->malloc(sizeof(int));
    if (!p) {
        host->conlog("Malloc failed\n");
    }
    *p = increment(b);

    host->push_data((char*)p, sizeof(int));
    host->free(p);

    host->conlog("Service terminating\n");

    return 0;
}


int increment (int a) {
    return a + 1;
}



