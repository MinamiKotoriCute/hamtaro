#pragma once

#include <string>
#include <map>
#include <boost/asio.hpp>

#include "awaitable.h"
#include "error_handle.h"

class TcpServer
{
public:
    using AcceptResultType = result<std::unique_ptr<boost::asio::ip::tcp::socket>>;

    TcpServer(boost::asio::io_context& io_context);
    TcpServer(boost::asio::strand<boost::asio::io_context::executor_type>& strand);

    result<void> listen(const std::string &ip, short port);
    void close();
    awaitable<AcceptResultType> accept();

private:
    boost::asio::ip::tcp::acceptor acceptor_;
};
