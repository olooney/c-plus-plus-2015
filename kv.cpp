#include <iostream>
#include <string>
#include <vector>
#include <exception>

namespace kv {

class UsageException: std::exception {
public:
    virtual const char* what() const throw() {
        return "Usage: kv get KEY\n"
               "       kv set KEY VALUE\n"
               "       kv del KEY";
    }
};

class Command {
public:
    enum class Verb { GET, SET, DEL };
    Verb verb;
    std::string key;
    std::string value;
};

Command parse_args(int argc, char** argv) {
    // args as vector, starting from 1 to skip command name
    std::vector<std::string> args(argv+1, argv+argc);

    if ( args.size() < 2 || args[1] == "help" ) {
        throw UsageException();
    }
    Command c;

    // first arg -> verb
    if ( args[0] == "get" ) {
        c.verb = Command::Verb::GET;
    } else if ( args[0] == "set" ) {
        c.verb = Command::Verb::SET;
    } else if ( args[0] == "del" ) {
        c.verb = Command::Verb::DEL;
    } else {
        throw UsageException();
    }   

    // second arg -> key
    c.key = args[1];

    // third arg (set only) is the value
    if ( c.verb == Command::Verb::SET ) {
        if ( args.size() != 3 ) {
            throw UsageException();
        } else {
            c.value = args[2];
        }
    } else {
        if ( args.size() != 2 ) {
            throw UsageException();
        }
    }

    return c;
}

} // end namespace kv

int main(int argc, char** argv) {
    
    try {
        kv::Command command = kv::parse_args(argc, argv);
        std::cout << static_cast<int>(command.verb) << " " << command.key << " " << command.value << std::endl;
    } catch ( kv::UsageException& e ) {
        std::cerr << e.what() << std::endl;
        return 0;
    } catch ( std::exception& e ) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    } catch ( ... ) {
        std::cerr << "nonstandard error!\n";
        return 2;
    }

    return 0;
}

