#include <iostream>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cerrno>
#include <cstring>
#include "errors.hpp"
#include "functions.hpp"
#include "structNclasses.hpp"
#include "shared_variables.hpp"


//called by parent process
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode) {
    if (argc < 3 || argc > 5) {
        std::cerr << "\033[31mERROR\033[0m use: " << argv[0] << " \033[4mfrom_username\033[0m \033[4mto_user_username\033[0m [\033[1m--bot\033[0m] [\033[1m--manuel\033[0m]." << std::endl;
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


//called by parent process
void display_information(){
    if (!args_ptr->manual_mode) {
        std::cerr << "\033[33mWARNING\033[0m you can CTRL+C before and after user joins to exit." << std::endl;
    } else {
        std::cerr << "\033[33mWARNING\033[0m manual mode enabled, press CTRL+C or send messages to receive messages from other user.\n\033[33mWARNING\033[0m you can CTRL+C before user connects or CTRL+D during your chat to exit." << std::endl;
    }
}


//called by parent process
bool contains_forbidden_characters(const std::string& str) {
    const std::string forbidden = "<>-[]/";
    for (char c : str) {
        if (forbidden.find(c) != std::string::npos) {
            return true;
        }
    }
    return false;
}


//called by parent process
void close_pipes(){
    close(fd_send);
    close(fd_receive);
}

//called by parent process 
void close_shared_memory(){
    munmap(shm_ptr, SHM_SIZE);
    unlink(SHM_NAME);
}



//called by parent process and child process
void output_shared_memory() {
    if (shm_ptr != nullptr) {
        shm_offset_ptr = static_cast<size_t*>(shm_ptr);
        size_t shm_offset = *shm_offset_ptr;

        char* shm_data = static_cast<char*>(shm_ptr) + sizeof(size_t);
        size_t offset = 0;

        std::cerr << "\n\033[33mWARNING\033[0m printing messages from shared memory.." << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------" << std::endl;
        while (offset < shm_offset) {
            char* message = shm_data + offset;
            std::cout << "[\x1B[4m" <<  args_ptr->to_user << "\x1B[0m] " << message << std::endl;
            offset += strlen(message) + 1; 
        }
        if (offset == 0) {
            std::cerr << "No messages in shared memory (you received 0 message)." << std::endl;
        }
        std::cout << "---------------------------------------------------------------------------------------------\n" << std::endl;
        //reset shared memory
        size_t* shm_offset_ptr = static_cast<size_t*>(shm_ptr);
        *shm_offset_ptr = 0;

    } else {
        std::cerr << "\033[31mERROR\033[0m shared memory is not initialized, user is not connected yet." << std::endl;
    }

}


//called by parent process
void handle_sigint(int sig) {
    if (args_ptr->manual_mode) {
        if (shm_ptr != nullptr) { //uses of shared memory to know if pipes are open because shared memory is opened after pipes
            output_shared_memory();
        }else{
            std::cerr << "\n\033[33mWARNING\033[0m " << args_ptr->to_user <<" is not connected, closing chat." << std::endl;
            pipes_ptr->cleanup_named_pipes(pipes_ptr->get_from_pipe(), pipes_ptr->get_to_pipe()); 
            exit(ERR_SIGNAL_HANDLING); //error code 4 because pipes not open
        }

    } else {
        std::cerr << "\n\033[33mWARNING\033[0m signal SIGINT received, closing chat." << std::endl;
        if (shm_ptr != nullptr) { //user is connected
            close_shared_memory();
            exit(SUCCESS);
        }else{ //user is not connected and pipes are not open
            pipes_ptr->cleanup_named_pipes(pipes_ptr->get_from_pipe(), pipes_ptr->get_to_pipe());
            exit(ERR_SIGNAL_HANDLING);
        }

    }
}


//called by parent process and child process
void handle_sigpipe(int sig) {
    std::cerr << "\033[33mWARNING\033[0m " << args_ptr->to_user << " disconnected, closing chat." << std::endl;
    //close shared memory of other program and named pipes before exiting
    close_shared_memory();
    close_pipes();
    if (pipes_ptr != nullptr) {
        pipes_ptr->cleanup_named_pipes(pipes_ptr->get_from_pipe(), pipes_ptr->get_to_pipe()); 
    }
    exit(SUCCESS); 
}


//called by parent process
void handle_ctrl_d() {
    std::cerr << "\n\033[33mWARNING\033[0m STDIN is closed, closing chat." << std::endl;
    //close shared memory of one program before exiting
    close_shared_memory();
    exit(SUCCESS);
}

//called by parent process and child process
void handle_pipe_error(const std::string& pipe_name) {
    std::cerr << "\033[31mERROR\033[0m with " << pipe << std::endl;
    exit(ERR_PIPE_RELATED);
}



//called by parent process and child process
SharedMemory creating_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "\033[31mERROR\033[0m opening shared memory: " << strerror(errno) << std::endl;
        exit(ERR_SHARED_MEMORY);
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "\033[31mERROR\033[0m configuring shared memory size: " << strerror(errno) << std::endl;
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(ERR_SHARED_MEMORY);
    }

    void* shm_ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::cerr << "\033[31mERROR\033[0m mapping shared memory: " << strerror(errno) << std::endl;
        close(shm_fd);
        exit(ERR_SHARED_MEMORY);
    }

    close(shm_fd);

    //initialize shm_offset to zero if this process created the shared memory
    size_t* shm_offset_ptr = static_cast<size_t*>(shm_ptr);
    *shm_offset_ptr = 0;

    return {shm_ptr, shm_offset_ptr};
}




//handled by parent process
void send_messages() {
    int fd_send = open(pipes_ptr->get_from_pipe().c_str(), O_WRONLY);
    if (fd_send == -1) {
        handle_pipe_error(pipes_ptr->get_from_pipe());
    }
    std::cerr << "\033[33mWARNING\033[0m " << args_ptr->to_user << " is connected, you can start chatting." << std::endl;

    //opening shared memory in parent process
    SharedMemory shared_memory = creating_shared_memory();
    shm_ptr = shared_memory.shm_ptr;
    shm_offset_ptr = shared_memory.shm_offset_ptr;

    std::string message;
    while (true) {
        if (!std::getline(std::cin, message)) {
            handle_ctrl_d();
        }
        if (write(fd_send, message.c_str(), message.size()) == -1) {
            handle_pipe_error(pipes_ptr->get_from_pipe());
        }

        if (args_ptr->manual_mode) {
            std::cout << "[\x1B[4m" << args_ptr->from_user << "\x1B[0m] " << message << std::endl;
            output_shared_memory();
        } else if (args_ptr->bot_mode) {
           //do nothing
        } else {
            std::cout << "[\x1B[4m" << args_ptr->from_user << "\x1B[0m] " << message << std::endl;
        }

    }

}




//handled by child process
void receive_messages() {
    int fd_receive = open(pipes_ptr->get_to_pipe().c_str(), O_RDONLY);
    if (fd_receive == -1) {
        handle_pipe_error(pipes_ptr->get_to_pipe());
    }

    //opening shared memory in child process
    SharedMemory shared_memory = creating_shared_memory();
    shm_ptr = shared_memory.shm_ptr;
    shm_offset_ptr = shared_memory.shm_offset_ptr;

    
    char buffer[4096];
    ssize_t bytes_read;

    while (true) {
        bytes_read = read(fd_receive, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            handle_pipe_error(pipes_ptr->get_to_pipe());
        } else if (bytes_read == 0) {
            kill(getppid(), SIGPIPE); //user disconnected, prevent parent process to close chat
            break;
        }
        buffer[bytes_read] = '\0'; //null-terminate the string ensure no overflow   


        //MODE MANUAL
        if (args_ptr->manual_mode) {
            size_t message_length = bytes_read + 1; //include null terminator
            //check if there is enough space
            if (*shm_offset_ptr + message_length > SHM_SIZE) {
                std::cerr << "\n\033[33mWARNING\033[0m shared memory is full, releasing messages.." << std::endl;
                output_shared_memory();
                *shm_offset_ptr = 0; 
                continue;
            }

            char* shm_data = static_cast<char*>(shm_ptr) + sizeof(size_t) + *shm_offset_ptr;
            memcpy(shm_data, buffer, message_length);
            *shm_offset_ptr += message_length; //update the offset in shared memory

            std::cout << "\a"; //prevent user with sound
        //MODE BOT
        } else if (args_ptr->bot_mode) {
            std::cout << "["<<  args_ptr->to_user << "] " << buffer << std::endl;
            std::flush(std::cout); 
        //MODE NORMAL
        } else{
            std::cout << "[\x1B[4m" <<  args_ptr->to_user << "\x1B[0m] " << buffer << std::endl;
        }     
    }

}