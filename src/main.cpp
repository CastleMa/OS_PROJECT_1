#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "errors.hpp"
#include "functions.hpp"
#include "datastruct.hpp"

//global ariables
pid_t receive_pid;
NamedPipes* pipes_ptr = nullptr; //used in signal handling


int main(int argc, char* argv[]) {

    std::string from_user, to_user;
    bool manual_mode, bot_mode;

    // Verify arguments
    int result = verify_arguments(argc, argv, from_user, to_user, manual_mode, bot_mode);
    if (result != 0) {
        return result;
    }

    //signal handling
    signal(SIGINT, handle_sigint);
    signal(SIGPIPE, handle_sigpipe);


    //create named pipes
    NamedPipes pipes(from_user, to_user);
    pipes_ptr = &pipes;

    //args to pass to the processes
    ChatArgs args = {from_user, to_user, manual_mode, bot_mode};


    //child process to receive messages
    receive_pid = fork();
    if (receive_pid == -1) {
        std::cerr << "\033[31mERROR\033[0m while creating child process." << std::endl;
        return ERR_PROCESS_CREATION;
    } else if (receive_pid == 0) {
        signal(SIGINT, SIG_IGN); 
        receive_messages(&args);
        exit(SUCCESS);
    }

    //parent process to send messages
    send_messages(&args);

    return SUCCESS;

}