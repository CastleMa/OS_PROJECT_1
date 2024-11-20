#include "structNclasses.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "errors.hpp"
#include "shared_variables.hpp"



NamedPipes::NamedPipes(const std::string& from_user, const std::string& to_user) {
    create_named_pipes(from_user, to_user);
    from_pipe = "/tmp/" + from_user + "-" + to_user + ".chat";
    to_pipe = "/tmp/" + to_user + "-" + from_user + ".chat";
}

NamedPipes::~NamedPipes() {
    cleanup_named_pipes(from_pipe, to_pipe);
}

void NamedPipes::create_named_pipes(const std::string& from_user, const std::string& to_user) {
    std::string pipe_send = "/tmp/" + from_user + "-" + to_user + ".chat";
    std::string pipe_receive = "/tmp/" + to_user + "-" + from_user + ".chat";

    //check if the send pipe already exists
    if (access(pipe_send.c_str(), F_OK) == 0) {
        std::cerr << "\033[33mWARNING\033[0m send pipe already exists: " << pipe_send << std::endl;
    } else {
        if (mkfifo(pipe_send.c_str(), 0666) == -1) {
            std::cerr << "\033[31mERROR\033[0m creating send pipe: " << pipe_send << std::endl;
            exit(ERR_PIPE_RELATED);
        } else {
        std::cerr << "\033[32mSuccess\033[0m creating send pipe: " << pipe_send << std::endl;
        }

    } 
    //check if the receive pipe already exists
    if (access(pipe_receive.c_str(), F_OK) == 0) {
        std::cerr << "\033[33mWARNING\033[0m receive pipe already exists: " << pipe_receive << std::endl;
    } else {
        if (mkfifo(pipe_receive.c_str(), 0666) == -1) {
            std::cerr << "\033[31mERROR\033[0m creating receive pipe: " << pipe_receive << std::endl;
            exit(ERR_PIPE_RELATED);
        } else {
            std::cerr << "\033[32mSuccess\033[0m creating receive pipe: " << pipe_receive << std::endl;
        }
    } 
}


void NamedPipes::cleanup_named_pipes(const std::string& from_pipe, const std::string& to_pipe) {

    if (unlink(from_pipe.c_str()) == -1) {
        std::cerr << "\033[33mWARNING\033[0m send pipe already removed: " << from_pipe << std::endl;
    } else {
        std::cerr << "\033[32mSuccess\033[0m removing send pipe: " << from_pipe << std::endl;
    }

    if (unlink(to_pipe.c_str()) == -1) {
        std::cerr << "\033[33mWARNING\033[0m receive pipe already removed: " << to_pipe << std::endl;
    } else {
        std::cerr << "\033[32mSuccess\033[0m removing receive pipe: " << to_pipe << std::endl;
    }
}



