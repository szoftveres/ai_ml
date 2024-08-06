#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <stdarg.h>

#include <dlfcn.h>
#include <gnu/lib-names.h>

#include "serviceapi.h"

size_t fsize (const char *filename) {
    struct stat st;

    if (!stat(filename, &st)) {
        return st.st_size;
    }
    return -1;
}

void binload (const char* filename, size_t len, char* buf) {
    int fp = open(filename, O_RDONLY);
    if (fp <= 0) {
        printf("Error opening %s\n", filename);
        return;
    }

    while (len) {
        int r = read(fp, buf, len);
        if (r < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                printf("%s\n", strerror(errno));
                exit(1);
            }
        } else if (!r) {
            printf("%s\n", strerror(errno));
            exit(0);
        } else {
            len -= r;
            buf += r;
        }
    }
    close(fp);
    printf("Binary loaded\n");
}


static int num;

int pull (char* buf, int len) {
    memcpy(buf, &num, len);
    printf("data requested, sending: %i\n", num);
    return 0;
}

int push (char* buf, int len) {
    int rc;
    memcpy(&rc, buf, len);
    printf("data received: %i\n", rc);
    return 0;
}


int
main (void) {

    serviceapi_t api = {
        .pull_data = pull,
        .push_data = push
    };

    void* cspace;
    int rc;

    size_t binfsize;
    char* binfname = "service/service.bin";

    servicefn_t fp;

    binfsize = fsize(binfname);

    printf("Bin size: %u\n", binfsize);

    /* Allocating an area that can be written as well as executed from */
    cspace = mmap(NULL, binfsize,
                  PROT_EXEC | PROT_READ |
                  PROT_WRITE, MAP_PRIVATE |
                  MAP_ANONYMOUS, -1, 0);


    binload(binfname, binfsize, cspace);

    /* By convention the first function is the entry point */
    fp = cspace;


    num = 3;

    /* Passing all control to the loaded service code */
    rc = fp(&api);

    printf("rc: %i\n", rc);

    munmap(cspace, binfsize);
    return 0;
}



