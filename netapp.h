#pragma once

#include <utility>
#include <boost/asio.hpp>

class NetApp
{
public:
  NetApp(boost::asio::io_context& io_context, unsigned short port, unsigned N);

private:
  void do_accept();

  boost::asio::ip::tcp::acceptor m_acceptor;
  unsigned m_N;
};
