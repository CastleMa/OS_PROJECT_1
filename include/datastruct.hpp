#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP

#include <string>


//structs
struct ChatArgs {
    std::string from_user;
    std::string to_user;
    bool manual_mode;
    bool bot_mode;
};




// RAII class for managing named pipes
class NamedPipes {
public:
    NamedPipes(const std::string& from_user, const std::string& to_user);
    ~NamedPipes();

private:
    std::string from_pipe;
    std::string to_pipe;

    int create_named_pipes(const std::string& from_user, const std::string& to_user);
    int cleanup_named_pipes(const std::string& from_pipe, const std::string& to_pipe);
};

#endif //structs or classes are already defined