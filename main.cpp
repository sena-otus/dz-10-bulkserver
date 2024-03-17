#include "async.h"

/* utility must be included before boost/asio.hpp because of the bug in boost */
#include <utility>
#include <boost/asio.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>


const int generic_errorcode = 200;


using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket, unsigned N)
    : m_socket(std::move(socket)), m_handle(async::connect(N))
  {
  }

  ~session()
  {
    async::disconnect(m_handle);
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    m_socket.async_read_some(
      boost::asio::buffer(m_data, max_length),
      [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::cout << "receive " << length << "=" << std::string{m_data.data(), length} << std::endl;
            async::receive(m_handle, m_data.data(), length);
//            do_write(length);
          }
        });
  }

  // void do_write(std::size_t length)
  // {
  //   auto self(shared_from_this());
  //   boost::asio::async_write(m_socket, boost::asio::buffer(m_data, length),
  //       [this, self](boost::system::error_code ec, std::size_t /*length*/)
  //       {
  //         if (!ec)
  //         {
  //           do_read();
  //         }
  //       });
  // }

  tcp::socket m_socket;
  enum { max_length = 1024 };
  std::array<char, max_length> m_data;
  async::handle_t m_handle;
};

class server
{
public:
  server(boost::asio::io_context& io_context, unsigned short port, unsigned N)
    : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)), m_N{N}
  {
    do_accept();
  }

private:
  void do_accept()
  {
    m_acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket), m_N)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor m_acceptor;
  unsigned m_N;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cout << "Usage\n"
                <<"  bulk_server <tcp port> <block size>\n"
                << "\n"
                << "Where"
                << "  <tcp port>   is tcp port to listen for incoming connections\n"
                << "  <block size> is command block size\n"
                << "\n"
                << "Example"
                << "  bulk_server 9300 3\n";
      return 0;
    }

    boost::asio::io_context io_context;



    server server(io_context, std::stoul(argv[1]), std::stoul(argv[2]));

    io_context.run();
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Exception: " << ex.what() << "\n";
    return generic_errorcode;
  }

  return 0;
}
