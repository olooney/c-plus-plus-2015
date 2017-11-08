#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;
using boost::starts_with;

struct character_escaper
{
    template<typename FindResultT>
    std::string operator()(const FindResultT& Match) const
    {
        std::string s;
        for (typename FindResultT::const_iterator i = Match.begin();
             i != Match.end();
             i++) {
            s += str(boost::format("\\x%02x") % static_cast<int>(*i));
        }
        return s;
    }
};

std::string escape(std::string s) {
    boost::find_format_all(s, boost::token_finder(!boost::is_print()), character_escaper());
    return s;
}

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket)
    : socket(std::move(socket)),
      value("42")
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::string response;
            std::string command = escape(std::string(data_, length));
            std::cout << "read " << length << " " << command << std::endl;
            if ( starts_with(command, "GET") ) {
                response = std::string("GOT ") + value + "\n";
            } else if ( starts_with(command, "SET ") ) {
                value = command.substr(4);
                response = "SAT\n";
            } else if ( starts_with(command, "DIE") ) {
                exit(0);
            } else {
                response = "ERROR: unknown command\n";
            }
            respond(response);
          } else {
            std::cout << "non-zero error code while reading from socket" << std::endl;
          }
        });
  }

  void respond(std::string response)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket, boost::asio::buffer(response),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::cout << "wrote " << length << " characters" << std::endl;
            do_read();
          } else {
            std::cout << "error writing to socket" << std::endl;
            // TODO return to do_read() here?
          }
        });
  }

  tcp::socket socket;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string value;

};

class server {
public:
  server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
    : acceptor(io_service, endpoint),
      socket(io_service)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor.async_accept(socket,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::cout << "starting new session" << std::endl;
            std::make_shared<session>(std::move(socket))->start();
          } {
            std::cout << "error accepting socket connection" << std::endl;
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor;
  tcp::socket socket;
};

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;

    tcp::endpoint server_endpoint(tcp::v4(), 8181);
    server s(io_service, server_endpoint);

    std::cout << "running" << std::endl;
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
