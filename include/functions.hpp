#ifndef MAIN_FUNCTIONS_HPP
#define MAIN_FUNCTIONS_HPP

#include <string>
#include "structNclasses.hpp"

bool contains_forbidden_characters(const std::string& str);
void handle_sigint(int sig);
void handle_sigpipe(int sig);
void display_information();
void* send_messages();
void* receive_messages();
void output_shared_memory();
SharedMemory opening_shared_memory();
void handle_ctrl_d();
void handle_pipe_error(const std::string& pipe_name);
int verify_arguments(int argc, char* argv[], std::string& from_user, std::string& to_user, bool& manual_mode, bool& bot_mode);

#endif 