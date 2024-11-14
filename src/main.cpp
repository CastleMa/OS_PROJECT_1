#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "errors.hpp"
#include "functions.hpp"
#include "struct.hpp"



int main(int argc, char* argv[]) {
    //too much args verif
    if (argc < 3 || argc > 5) {
        std::cerr << "\033[31mERROR\033[0m\nUse: " << argv[0] << " from_user_pseudo to_user_pseudo [\033[1m--bot\033[0m] [\033[1m--manuel\033[0m]." << std::endl;
        return ERR_INVALID_ARGS;
    }

    std::string from_user = argv[1];
    std::string to_user = argv[2]; 
    bool manual_mode = false;
    bool bot_mode    = false;

    //forbidden character verif
    if (contains_forbidden_characters(from_user) || contains_forbidden_characters(to_user)) {
    std::cerr << "\033[31mERROR\033[0m\nArguments must not contain <, >, -, [, or ]." << std::endl;
    return ERR_INVALID_CHARACTER;
    }

    //too long verif
    if (from_user.size() > 30 || to_user.size() > 30) {
        std::cerr << "\033[31mERROR\033[0m\nUsername too long." << std::endl;
        return ERR_USERNAME_TOO_LONG;
    }

    //options verif
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




    // Gestion des signaux
    signal(SIGINT, handle_sigint);

    // Création des pipes nommés
    create_named_pipes(from_user, to_user);

    // Arguments pour les threads
    ChatArgs args = {from_user, to_user, manual_mode};
    pthread_t send_thread, receive_thread;

    // Création des threads pour envoyer et recevoir des messages
    if (pthread_create(&send_thread, nullptr, send_messages, &args) != 0) {
        std::cerr << "Error while creating thread." << std::endl;
        return ERR_THREAD_CREATION;
    }
    if (pthread_create(&receive_thread, nullptr, receive_messages, &args) != 0) {
        std::cerr << "Error while creating thread." << std::endl;
        return ERR_THREAD_CREATION;
    }

    // Attendre la fin des threads
    pthread_join(send_thread, nullptr);
    pthread_join(receive_thread, nullptr);

    // Nettoyage des pipes nommés à la fin
    cleanup_named_pipes(from_user, to_user);

    return EXIT_SUCCESS;
}

