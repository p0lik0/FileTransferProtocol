#ifndef _REQUEST_H_
#define _REQUEST_H_

typedef enum typereq_t {GET, PUT, LS, NONDEF}typereq_t;

typedef struct{
    int type;
    char* filename;
} request_t;

#endif