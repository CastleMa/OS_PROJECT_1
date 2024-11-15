#include "datastruct.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "errors.hpp"
#include "datastruct.hpp"

// Constructor
NamedPipes::NamedPipes(const std::string& from_user, const std::string& to_user) {
    create_named_pipes(from_user, to_user);
    from_pipe = "/tmp/" + from_user + "-" + to_user + ".chat";
    to_pipe = "/tmp/" + to_user + "-" + from_user + ".chat";
}

// Destructor
NamedPipes::~NamedPipes() {
    cleanup_named_pipes(from_pipe, to_pipe);
}

// Create named pipes
int NamedPipes::create_named_pipes(const std::string& from_user, const std::string& to_user) {
    std::string pipe_send = "/tmp/" + from_user + "-" + to_user + ".chat";
    std::string pipe_receive = "/tmp/" + to_user + "-" + from_user + ".chat";

    if (mkfifo(pipe_send.c_str(), 0666) == -1) {
        std::cerr << "\033[31mERROR\033[0m creating send pipe: " << pipe_send << std::endl;
        return ERR_PIPE_CREATION;
    }

    if (mkfifo(pipe_receive.c_str(), 0666) == -1) {
        std::cerr << "\033[31mERROR\033[0m creating receive pipe: " << pipe_receive << std::endl;
        return ERR_PIPE_CREATION;

    }
    return SUCCESS;
}

// Cleanup named pipes
int NamedPipes::cleanup_named_pipes(const std::string& from_pipe, const std::string& to_pipe) {
    if (unlink(from_pipe.c_str()) == -1) {
        std::cerr << "\033[31mERROR\033[0m removing send pipe: " << from_pipe << std::endl;
        return ERR_PIPE_DELETE;

    }
    if (unlink(to_pipe.c_str()) == -1) {
        std::cerr << "\033[31mERROR\033[0m removing receive pipe: " << to_pipe << std::endl;
        return ERR_PIPE_DELETE;

    }

    return SUCCESS;
}