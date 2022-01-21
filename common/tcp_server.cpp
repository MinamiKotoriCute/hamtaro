#include "tcp_server.h"

#include <glog/logging.h>
#include "url_parser.h"

TcpServer::TcpServer(boost::asio::io_context& io_context) :
    acceptor_(io_context)
{

}

TcpServer::TcpServer(boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
    acceptor_(strand)
{

}

result<void> TcpServer::listen(const std::string &ip, short port)
{
    if (acceptor_.is_open())
    {
        return RESULT_ERROR("already listen");
    }

    boost::system::error_code ec;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(ip, ec);
    if (ec)
    {
        return RESULT_ERROR("make ip adress fail. what:") << ec.message();
    }
    auto endpoint = boost::asio::ip::tcp::endpoint(ip_address, port);
    
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        return RESULT_ERROR("acceptor open fail. what:") << ec.message();
    }

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));

    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        return RESULT_ERROR("bind endpoint fail. what:") << ec.message();
    }

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        return RESULT_ERROR("bind listen fail. what:") << ec.message();
    }
    
    return RESULT_SUCCESS;
}

void TcpServer::close()
{
    acceptor_.close();
}

awaitable<TcpServer::AcceptResultType> TcpServer::accept()
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::socket socket = co_await acceptor_.async_accept(boost::asio::redirect_error(boost::asio::use_awaitable, ec));

    if (ec)
    {
        co_return RESULT_ERROR("bind listen fail. what:") << ec.message();
    }

    auto p = std::make_unique<boost::asio::ip::tcp::socket>(acceptor_.get_executor());
    (*p.get()) = std::move(socket);
    co_return p;
}
