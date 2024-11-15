#include <iostream>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "errors.hpp"
#include "errors.hpp"
#include "functions.hpp"
#include "datastruct.hpp"


// Function to verify arguments
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode) {
    if (argc < 3 || argc > 5) {
        std::cerr << "\033[31mERROR\033[0m\nUse: " << argv[0] << " \033[4mfrom_user_pseudo\033[0m \033[4mto_user_pseudo\033[0m [\033[1m--bot\033[0m] [\033[1m--manuel\033[0m]." << std::endl;
        return ERR_INVALID_ARGS;
    }

    from_user = argv[1];
    to_user = argv[2];
    manual_mode = false;
    bot_mode = false;

    if (contains_forbidden_characters(from_user) || contains_forbidden_characters(to_user)) {
        std::cerr << "\033[31mERROR\033[0m\nArguments must not contain <, >, -, [, or ]." << std::endl;
        return ERR_INVALID_CHARACTER;
    }

    if (from_user.size() > 30 || to_user.size() > 30) {
        std::cerr << "\033[31mERROR\033[0m\nUsername too long." << std::endl;
        return ERR_USERNAME_TOO_LONG;
    }

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--manuel") {
            manual_mode = true;
        } else if (arg == "--bot") {
            bot_mode = true;
        } else {
            std::cerr << "\033[31mERROR\033[0m\nChoose between these options: [\033[1m--bot\033[0m], [\033[1m--manuel\033[0m]. Not: '" << arg << "'." << std::endl;
            return ERR_INVALID_ARGS;
        }
    }

    return SUCCESS; 

}

// Function to check if a string contains forbidden characters
bool contains_forbidden_characters(const std::string& str) {
    const std::string forbidden = "<>-[]/";
    for (char c : str) {
        if (forbidden.find(c) != std::string::npos) {
            return true;
        }
    }
    return false;
}



// Function to create and manage threads
int create_threads(ChatArgs& args, pthread_t& send_thread, pthread_t& receive_thread) {
    if (pthread_create(&send_thread, nullptr, send_messages, &args) != 0) {
        std::cerr << "\033[31mERROR\033[0m while creating thread." << std::endl;
        return ERR_THREAD_CREATION;
    }
    if (pthread_create(&receive_thread, nullptr, receive_messages, &args) != 0) {
        std::cerr << "\033[31mERROR\033[0m while creating thread." << std::endl;
        return ERR_THREAD_CREATION;
    }

}

// Fonction pour gérer le signal SIGINT
void handle_sigint(int sig) {
    std::cout << "\nSignal SIGINT reçu. Fermeture du chat." << std::endl;
    exit(0);
}


// Fonction pour envoyer des messages
void* send_messages(void* arg) {
    ChatArgs* args = static_cast<ChatArgs*>(arg);
    // Logique pour envoyer des messages
    // Utiliser args->user et args->dest
    return nullptr;
}

// Fonction pour recevoir des messages
void* receive_messages(void* arg) {
    ChatArgs* args = static_cast<ChatArgs*>(arg);
    // Logique pour recevoir des messages
    // Utiliser args->user et args->dest
    return nullptr;
}


