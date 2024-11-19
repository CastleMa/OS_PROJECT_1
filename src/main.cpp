#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "errors.hpp"
#include "functions.hpp"
#include "structNclasses.hpp"
#include "shared_variables.hpp"


//global variables 
NamedPipes* pipes_ptr = nullptr;
void* shm_ptr = nullptr;
size_t* shm_offset_ptr = nullptr;
ChatArgs* args_ptr = nullptr;
std::string pid = "/chat_shm" + std::to_string(getpid()); //gets unique pid to not conflict with other program shared memory
const char* SHM_NAME = pid.c_str();
int fd_send, fd_receive;

int main(int argc, char* argv[]) {

    std::string from_user, to_user;
    bool manual_mode, bot_mode;

    //argument verification
    int result = verify_arguments(argc, argv, from_user, to_user, manual_mode, bot_mode);
    if (result != 0) {
        return result;
    }


    //args to pass to the processes
    ChatArgs args = {from_user, to_user, manual_mode, bot_mode};
    args_ptr = &args;

    //display information about the chat
    display_information();

    //signal handling
    signal(SIGINT, handle_sigint);
    signal(SIGPIPE, handle_sigpipe);


    //create named pipes and open them
    NamedPipes pipes(from_user, to_user);
    pipes_ptr = &pipes;


    //cild process to receive messages
    pid_t receive_pid = fork();
    if (receive_pid == -1) {
        std::cerr << "\033[31mERROR\033[0m while creating child process." << std::endl;
        exit(ERR_PROCESS_CREATION);
    } else if (receive_pid == 0) {
        signal(SIGINT, SIG_IGN); //ignore SIGINT in the child process
        receive_messages();
        exit(SUCCESS);
    }

    //parent process to send messages
    send_messages();

    return SUCCESS;

}