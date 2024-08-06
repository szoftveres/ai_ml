#include "../serviceapi.h"

int increment (int a);




/* By convention the entry point is the first function */

int _start (serviceapi_t* api) {
    int b;

    api->pull_data(&b, sizeof(b));

    b = increment(b);

    api->push_data(&b, sizeof(b));

    return 0;
}


int increment (int a) {
    return a+1;
}



