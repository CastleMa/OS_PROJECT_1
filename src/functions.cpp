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




void display_information(){

    if (args_ptr->manual_mode) {
        std::cout << "\033[33mWARNING\033[0m manual mode enabled, press CTRL+C or send messages to receive messages from other user\nYou can CTRL+C before user joins or CTRL+D during your chat to exit." << std::endl;
    }


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
    if (args_ptr->manual_mode) {
        std::cout << "\n\033[33mWARNING\033[0m signal SIGINT received.." << std::endl;
        output_shared_memory();
    } else {
        std::cout << "\n\033[33mWARNING\033[0m signal SIGINT received and no --manuel mode, closing chat.." << std::endl;
        //close shared memory and file descriptors
        close(fd_send);
        close(fd_receive);
        munmap(shm_ptr, SHM_SIZE);
        unlink(SHM_NAME);
        
        exit(SUCCESS);
    }
}


//handled by parent process and child process
void output_shared_memory() {
    if (shm_ptr != nullptr) {
        shm_offset_ptr = static_cast<size_t*>(shm_ptr);
        size_t shm_offset = *shm_offset_ptr;

        char* shm_data = static_cast<char*>(shm_ptr) + sizeof(size_t);
        size_t offset = 0;

        std::cout << "\n----------------------------------------" << std::endl;
        std::cout << "\033[33mWARNING\033[0m printing messages from shared memory.." << std::endl;
        while (offset < shm_offset) {
            char* message = shm_data + offset;
            std::cout << "[\x1B[4m" +  args_ptr->to_user + "\x1B[0m] " << message << std::endl;
            offset += strlen(message) + 1; 
        }
        if (offset == 0) {
            std::cerr << "No messages in shared memory (you received 0 message)." << std::endl;
        }
        std::cout << "----------------------------------------\n" << std::endl;
        //reset shared memory
        size_t* shm_offset_ptr = static_cast<size_t*>(shm_ptr);
        *shm_offset_ptr = 0;

    } else {
        std::cerr << "\033[31mERROR\033[0m shared memory is not initialized." << std::endl;
    }

}


//handled by parent process when other user disconnects
void handle_sigpipe(int sig) {
    std::cerr << "\033[33mWARNING\033[0m user disconnected, removing pipes and closing chat.." << std::endl;

    //close shared memory and file descriptors before removing pipes
    close(fd_send);
    close(fd_receive);
    munmap(shm_ptr, SHM_SIZE);
    unlink(SHM_NAME);

    if (pipes_ptr != nullptr) {
        pipes_ptr->cleanup_named_pipes(pipes_ptr->get_from_pipe(), pipes_ptr->get_to_pipe()); 
    }

    exit(SUCCESS); 
}



//handled by parent process
void handle_ctrl_d() {
    std::cout << "\n\033[33mWARNING\033[0m STDIN is closed, closing chat.." << std::endl;
    exit(SUCCESS);
}

//handled by parent process and child process
void handle_pipe_error(const std::string& pipe_name) {
    std::cerr << "\033[31mERROR\033[0m with " << pipe << std::endl;
    exit(ERR_PIPE_WRITE_READ_OPENING);
}


//handled by parent process and child process
SharedMemory opening_shared_memory() {
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

    //defining shared memory in parent process
    SharedMemory shared_memory = opening_shared_memory();
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
        std::cout << "[\x1B[4m" + args_ptr->from_user + "\x1B[0m] " << message << std::endl;

        if (args_ptr->manual_mode) {
            output_shared_memory();
        }

    }

}


//handled by child process
void receive_messages() {

    int fd_receive = open(pipes_ptr->get_to_pipe().c_str(), O_RDONLY);
    if (fd_receive == -1) {
        handle_pipe_error(pipes_ptr->get_to_pipe());
    }

    //defining shared memory in child process
    SharedMemory shared_memory = opening_shared_memory();
    shm_ptr = shared_memory.shm_ptr;
    shm_offset_ptr = shared_memory.shm_offset_ptr;

    
    char buffer[4096];
    ssize_t bytes_read;

    while (true) {
        bytes_read = read(fd_receive, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            handle_pipe_error(pipes_ptr->get_to_pipe());
        } else if (bytes_read == 0) {
            kill(getppid(), SIGPIPE);
            break;
        }
        buffer[bytes_read] = '\0'; //null-terminate the string ensure no overflow   


        //MODE MANUEL
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

            //prevent user with sound
            std::cout << "\a";
        } else {
            std::cout << "[\x1B[4m" +  args_ptr->to_user + "\x1B[0m] " << buffer << std::endl;
        }     
    }

}