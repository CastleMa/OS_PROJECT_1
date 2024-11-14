#include <iostream>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "functions.hpp"
#include "struct.hpp"




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


// Fonction pour gérer le signal SIGINT
void handle_sigint(int sig) {
    std::cout << "\nSignal SIGINT reçu. Fermeture du chat." << std::endl;
    exit(0);
}

// Fonction pour créer les pipes nommés
void create_named_pipes(const std::string& user, const std::string& dest) {
    std::string pipe_send = "/tmp/" + user + "-" + dest + ".chat";
    std::string pipe_receive = "/tmp/" + dest + "-" + user + ".chat";

    mkfifo(pipe_send.c_str(), 0666);
    mkfifo(pipe_receive.c_str(), 0666);
}

// Fonction pour nettoyer les pipes nommés
void cleanup_named_pipes(const std::string& user, const std::string& dest) {
    std::string pipe_send = "/tmp/" + user + "-" + dest + ".chat";
    std::string pipe_receive = "/tmp/" + dest + "-" + user + ".chat";

    unlink(pipe_send.c_str());
    unlink(pipe_receive.c_str());
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


