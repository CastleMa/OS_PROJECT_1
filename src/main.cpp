#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <boost/interprocess/ipc/message_queue.hpp>

// Déclarations de fonctions
void handle_sigint(int sig);
void create_named_pipes(const std::string& user, const std::string& dest);
void cleanup_named_pipes(const std::string& user, const std::string& dest);
void* send_messages(void* arg);
void* receive_messages(void* arg);

// Structure pour passer des arguments aux threads
struct ChatArgs {
    std::string user;
    std::string dest;
    bool manual_mode;
};

int main(int argc, char* argv[]) {
    // Vérification des paramètres
    if (argc < 3 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " pseudo_utilisateur pseudo_destinataire [--bot] [--manuel]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string user = argv[1];
    std::string dest = argv[2];
    bool manual_mode = false;

    // Vérification des options
    for (int i = 3; i < argc; i++) {
        if (std::strcmp(argv[i], "--manuel") == 0) {
            manual_mode = true;
        }
        // Vous pouvez ajouter d'autres options ici
    }

    // Gestion des signaux
    signal(SIGINT, handle_sigint);

    // Création des pipes nommés
    create_named_pipes(user, dest);

    // Arguments pour les threads
    ChatArgs args = {user, dest, manual_mode};
    pthread_t send_thread, receive_thread;

    // Création des threads pour envoyer et recevoir des messages
    pthread_create(&send_thread, nullptr, send_messages, &args);
    pthread_create(&receive_thread, nullptr, receive_messages, &args);

    // Attendre la fin des threads
    pthread_join(send_thread, nullptr);
    pthread_join(receive_thread, nullptr);

    // Nettoyage des pipes nommés à la fin
    cleanup_named_pipes(user, dest);

    return EXIT_SUCCESS;
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


