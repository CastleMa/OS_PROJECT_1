#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP

#include <string>

struct ChatArgs {
    std::string from_user;
    std::string to_user;
    bool manual_mode;
    bool bot_mode;
};

class NamedPipes {
public:
    NamedPipes(const std::string& from_user, const std::string& to_user);
    ~NamedPipes();

    std::string get_from_pipe() const { return from_pipe; }
    std::string get_to_pipe() const { return to_pipe; }

    void cleanup_named_pipes(const std::string& from_pipe, const std::string& to_pipe);

private:
    std::string from_pipe;
    std::string to_pipe;

    void create_named_pipes(const std::string& from_user, const std::string& to_user);
};

#endif 