#ifndef MAIN_FUNCTIONS_HPP
#define MAIN_FUNCTIONS_HPP

#include <string>
#include "struct.hpp"

//function declarations

bool contains_forbidden_characters(const std::string& str);
void handle_sigint(int sig);
void create_named_pipes(const std::string& user, const std::string& dest);
void cleanup_named_pipes(const std::string& user, const std::string& dest);
int create_threads(ChatArgs& args, pthread_t& send_thread, pthread_t& receive_thread);
void* send_messages(void* arg);
void* receive_messages(void* arg);
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode);

#endif // main functions are already defined 