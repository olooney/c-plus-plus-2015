#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <map>
#include <fstream>
#include <algorithm>

namespace kv {

class UsageException: std::exception {
public:
    virtual const char* what() const throw() {
        return "Usage: kv get KEY\n"
               "       kv set KEY VALUE\n"
               "       kv del KEY";
    }
};

class FileException: std::exception {
public:
    virtual const char* what() const throw() {
        return "Unable to access .kvcache file";
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

using KeyValueMap = std::map<std::string, std::string>;

// returns a new string with leading and trailing whitespace removed
std::string strip(const std::string& s) {
    auto not_space = [](int c) { return !std::isspace(c); };
    auto start = std::find_if(s.begin(), s.end(), not_space);
    auto end = std::find_if(s.rbegin(), s.rend(), not_space);
    return std::string(start, end.base());
}

KeyValueMap load() {
    KeyValueMap map;
    std::string line;
    std::ifstream fin(".kvcache");
    if ( fin.is_open() ) {
        while ( std::getline(fin, line) ) {
            // scan forward for a comment character
            auto comment = std::find(line.begin(), line.end(), '#');

            // strip off the comment
            line.erase(comment, line.end());

            // remove whitespace
            line = strip(line);

            // skip blank lines (or comment lines)
            if ( line.empty() ) {
                continue;
            }
            
            auto delim = std::find(line.begin(), line.end(), '=');
            if ( delim == line.end() ) {
                continue; // TODO: error?
            }

            std::string key = strip(std::string(line.begin(), delim));
            std::string value = strip(std::string(delim+1, line.end()));
            if ( !key.empty() ) {
                map[key] = value;
            }
        }
    }
    return map;
}

void dump(const KeyValueMap& map) {
    std::ofstream fout(".kvcache");

    // unlike load(), creating the file is not optional 
    // because we have a value we need to write.
    if ( !fout.is_open() ) {
        throw FileException();
    }

    // write one "key = value" pair per line
    for ( auto ppair = map.begin(); ppair != map.end(); ppair++ ) {
        fout << ppair->first << " = " << ppair->second << std::endl;        
    }

    fout.close();
}

namespace handler {
    int get(const Command& command) {
        auto map = kv::load();
        auto ppair = map.find(command.key);
        if ( ppair == map.end() ) {
            return 1;
        } else {
            std::string value = ppair->second;
            std::cout << value << std::endl;
            return 0;
        }
    }

    int set(const Command& command) {
        auto map = load();
        map[ command.key ] = command.value;
        dump(map);
        return 0;
    }

    int del(const Command& command) {
        auto map = load();
        map.erase(command.key);
        dump(map);
        return 0;
    }
}

using CommandHandler = int(*)(const Command& command);

static const std::map<Command::Verb, CommandHandler> verbToHandler = {
    { Command::Verb::GET, kv::handler::get },
    { Command::Verb::SET, kv::handler::set },
    { Command::Verb::DEL, kv::handler::del },
};

} // end namespace kv

int main(int argc, char** argv) {
    
    try {
        // interpret command line arguments
        kv::Command command = kv::parse_args(argc, argv);

        // delegate to appropriate handler
        auto handler = kv::verbToHandler.at(command.verb);
        return handler(command);

    } catch ( kv::UsageException& e ) {
        std::cerr << e.what() << std::endl;
        return 0;
    } catch ( std::exception& e ) {
        std::cerr << "error: " << e.what() << std::endl;
        return 2;
    } catch ( ... ) {
        std::cerr << "nonstandard error!\n";
        return 3;
    }

    return 0;
}

