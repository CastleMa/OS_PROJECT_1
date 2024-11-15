#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "errors.hpp"
#include "functions.hpp"
#include "datastruct.hpp"

int main(int argc, char* argv[]) {
    std::string from_user, to_user;
    bool manual_mode, bot_mode;

    // Verify arguments
    int result = verify_arguments(argc, argv, from_user, to_user, manual_mode, bot_mode);
    if (result != 0) {
        return result;
    }

    // Signal handling
    signal(SIGINT, handle_sigint);

    // Create named pipes
    NamedPipes pipes(from_user, to_user);

    // Arguments for threads
    ChatArgs args = {from_user, to_user, manual_mode, bot_mode};

    pthread_t send_thread, receive_thread;

    // Create threads for sending and receiving messages
    result = create_threads(args, send_thread, receive_thread);
    if (result != 0) {
        return result;
    }

    // Wait for threads to finish
    pthread_join(send_thread, nullptr);
    pthread_join(receive_thread, nullptr);

    // Clean up named pipes at the end
    NamedPipes cleanup_named_pipes(from_user, to_user);

    return SUCCESS;
}
