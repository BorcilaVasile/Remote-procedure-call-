#include <RPC/errors.h>

void fail(const char* error_info){
    perror(error_info);
    exit(EXIT_FAILURE);
}