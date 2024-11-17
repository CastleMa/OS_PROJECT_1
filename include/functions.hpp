#ifndef MAIN_FUNCTIONS_HPP
#define MAIN_FUNCTIONS_HPP

#include <string>
#include "datastruct.hpp"

bool contains_forbidden_characters(const std::string& str);
void handle_sigint(int sig);
void handle_sigpipe(int sig);
void* send_messages(void* arg);
void* receive_messages(void* arg);
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode);

#endif 