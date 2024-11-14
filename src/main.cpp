#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "errors.hpp"
#include "functions.hpp"
#include "struct.hpp"







// RAII class for managing named pipes
class NamedPipes {
public:
    NamedPipes(const std::string& from_user, const std::string& to_user) {
        create_named_pipes(from_user, to_user);
        from_pipe = "/tmp/" + from_user + "-" + to_user + ".chat";
        to_pipe = "/tmp/" + to_user + "-" + from_user + ".chat";
    }

    ~NamedPipes() {
        cleanup_named_pipes(from_pipe, to_pipe);
    }

private:
    std::string from_pipe;
    std::string to_pipe;
};






int main(int argc, char* argv[]) {
    std::string from_user;
    std::string to_user;
    bool manual_mode;
    bool bot_mode;

    int verification_result = verify_arguments(argc, argv, from_user, to_user, manual_mode, bot_mode);
    if (verification_result != 0) {
        return verification_result;
    }


    // Gestion des signaux
    signal(SIGINT, handle_sigint);

    // Création des pipes nommés
    NamedPipes pipes(from_user, to_user);

    // Arguments pour les threads
    ChatArgs args = {from_user, to_user, manual_mode};
    ChatArgs args = {from_user, to_user, manual_mode};
    pthread_t send_thread, receive_thread;

    // Création des threads pour envoyer et recevoir des messages
    create_threads(args, send_thread, receive_thread);

    // Attendre la fin des threads
    pthread_join(send_thread, nullptr);
    pthread_join(receive_thread, nullptr);

    // Nettoyage des pipes nommés à la fin
    cleanup_named_pipes(from_user, to_user);

    return 0;
}

