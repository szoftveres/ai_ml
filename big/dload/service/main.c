#include "../hostapi.h"

int increment (int a);




/* By convention the entry point is the first function */

int _start (hostapi_t* host) {
    int b;

    host->pull_data((char*)&b, sizeof(b));

    b = increment(b);

    host->conlog("Service dynamically linked and running\n");

    host->push_data((char*)&b, sizeof(b));

    return 0;
}


int increment (int a) {
    return a+1;
}



