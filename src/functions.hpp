#ifndef MAIN_FUNCTIONS_HPP
#define MAIN_FUNCTIONS_HPP

#include <string>

//function declarations
bool contains_forbidden_characters(const std::string& str);
void handle_sigint(int sig);
void create_named_pipes(const std::string& user, const std::string& dest);
void cleanup_named_pipes(const std::string& user, const std::string& dest);
void* send_messages(void* arg);
void* receive_messages(void* arg);

#endif // main functions are already defined 