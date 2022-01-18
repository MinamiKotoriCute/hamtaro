#pragma once

#include <vector>
#include <boost/asio.hpp>

#include "error_handle.h"

class TcpClient
{
public:
    using ConnectResultType = result<void>;
    using WriteResultType = result<void>;
    using ReadResultType = result<void>;

    TcpClient(boost::asio::io_context& io_context);
    TcpClient(boost::asio::strand<boost::asio::io_context::executor_type>& strand);
    TcpClient(boost::asio::ip::tcp::socket socket);

    boost::asio::awaitable<ConnectResultType> connect(const std::string &url);
    void disconnect();
    boost::asio::awaitable<ReadResultType> read(std::vector<char> &read_buffer);
    boost::asio::awaitable<WriteResultType> write(std::vector<char> &&data);

private:
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	std::vector<char> read_buffer_;
	std::vector<char> write_buffer_;
};
