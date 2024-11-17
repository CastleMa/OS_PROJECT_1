#include <iostream>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "errors.hpp"
#include "functions.hpp"
#include "datastruct.hpp"
#include <cerrno>





//global variables
extern pid_t receive_pid;
extern NamedPipes* pipes_ptr; 




//handled by parent process
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode) {
    if (argc < 3 || argc > 5) {
        std::cerr << "\033[31mERROR\033[0m use: " << argv[0] << " \033[4mfrom_user_pseudo\033[0m \033[4mto_user_pseudo\033[0m [\033[1m--bot\033[0m] [\033[1m--manuel\033[0m]." << std::endl;
        return ERR_INVALID_ARGS;
    }

    from_user = argv[1];
    to_user = argv[2];
    manual_mode = false;
    bot_mode = false;

    if (contains_forbidden_characters(from_user) || contains_forbidden_characters(to_user)) {
        std::cerr << "\033[31mERROR\033[0m arguments must not contain <, >, -, [, or ]." << std::endl;
        return ERR_INVALID_CHARACTER;
    }

    if (from_user.size() > 30 || to_user.size() > 30) {
        std::cerr << "\033[31mERROR\033[0m username too long." << std::endl;
        return ERR_USERNAME_TOO_LONG;
    }

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--manuel") {
            manual_mode = true;
        } else if (arg == "--bot") {
            bot_mode = true;
        } else {
            std::cerr << "\033[31mERROR\033[0m choose between these options: [\033[1m--bot\033[0m], [\033[1m--manuel\033[0m]. Not: '" << arg << "'." << std::endl;
            return ERR_INVALID_ARGS;
        }
    }

    return SUCCESS;
}






//handled by parent process
bool contains_forbidden_characters(const std::string& str) {
    const std::string forbidden = "<>-[]/";
    for (char c : str) {
        if (forbidden.find(c) != std::string::npos) {
            return true;
        }
    }
    return false;
}






//handled by parent process
void handle_sigint(int sig) {
    std::cout << "\n\033[33mWARNING\033[0m signal SIGINT received, closing chat.." << std::endl;
    if (pipes_ptr != nullptr) {
        pipes_ptr->cleanup_named_pipes(pipes_ptr->get_from_pipe(), pipes_ptr->get_to_pipe()); // Call cleanup function using getter methods
    }
    exit(ERR_SIGNAL_HANDLING);
}






//handled by parent process
void handle_sigpipe(int sig) {
    std::cerr << "\033[33mWARNING\033[0m u ser disconnected, pipes removed, closing chat.." << std::endl;
    exit(SUCCESS); 
}






//handled by parent process
void* send_messages(void* arg) {
    ChatArgs* args = static_cast<ChatArgs*>(arg);
    std::string pipe_send = "/tmp/" + args->from_user + "-" + args->to_user + ".chat";
    int fd = open(pipe_send.c_str(), O_WRONLY);
    if (fd == -1) {
        std::cerr << "\033[31mERROR\033[0m opening send pipe: " << pipe_send << std::endl;
    }

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (write(fd, message.c_str(), message.size()) == -1) {
            if (errno == EPIPE) {
                std::cerr << "\033[31mERROR\033[0m send pipe closed: " << pipe_send << std::endl;
            } else {
                std::cerr << "\033[31mERROR\033[0m writing to send pipe: " << pipe_send << std::endl;
            }
            break;
        }
        std::cout << "[\x1B[4m" + args->from_user + "\x1B[0m] " << message << std::endl;
    }

    close(fd);
    return nullptr;
}








//handled by child process
void* receive_messages(void* arg) {
    ChatArgs* args = static_cast<ChatArgs*>(arg);
    std::string pipe_receive = "/tmp/" + args->to_user + "-" + args->from_user + ".chat";
    int fd = open(pipe_receive.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "\033[31mERROR\033[0m opening receive pipe: " << pipe_receive << std::endl;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while (true) {
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            std::cerr << "\033[31mERROR\033[0m reading from receive pipe: " << pipe_receive << std::endl;
            break;
        } else if (bytes_read == 0) {
            kill(getppid(), SIGPIPE);
            break;
        }
        buffer[bytes_read] = '\0';
        std::cout << "[\x1B[4m" + args->to_user + "\x1B[0m] "  << buffer << std::endl;
    }

    close(fd);
    return nullptr;
}