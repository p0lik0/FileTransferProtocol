#ifndef _REQUEST_STRUCT_H_
#define _REQUEST_STRUCT_H_

enum typereq_t {GET, PUT, LS};

typedef struct{
    typereq_t type;
    char* filename;
} request_t;

#endif