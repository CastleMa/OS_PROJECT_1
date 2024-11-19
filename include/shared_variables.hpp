#ifndef SHARED_VARIABLE_HPP
#define SHARED_VARIABLE_HPP

#include "structNclasses.hpp"
#include <string>


//shared variables between source files
constexpr size_t SHM_SIZE = 4096;
extern const char* SHM_NAME;
extern NamedPipes* pipes_ptr;
extern void* shm_ptr;
extern size_t* shm_offset_ptr;
extern ChatArgs* args_ptr;
extern int fd_send, fd_receive;

#endif 