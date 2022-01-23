#pragma once

#include <map>
#include <memory>
#include <google/protobuf/message.h>

#include "awaitable.h"
#include "error_handle.h"
#include "tcp_server.h"

template<typename PackCoder, typename PackTcpReader>
class basic_ProtobufTcpServer
{
public:
    using ClientType = basic_ProtobufTcpClient<PackCoder, PackTcpReader>;
    using ListenCallbackType = std::function<result<void>(ClientType&)>;

    basic_ProtobufTcpServer(boost::asio::io_context& io_context) :
        tcp_server_(io_context)
    {
    }

    basic_ProtobufTcpServer(boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
        tcp_server_(strand)
    {
    }

    result<void> listen(const std::string &ip, short port)
    {
        RESULT_CHECK(tcp_server_.listen(ip, port));

        return RESULT_SUCCESS;
    }

    void close()
    {
        tcp_server_.close();
    }

    awaitable<result<std::unique_ptr<ClientType>>> accept()
    {
        RESULT_CO_AUTO(tcp_client, co_await tcp_server_.accept());

        co_return std::make_unique<ClientType>(std::move(*tcp_client.get()));
    }

private:
    TcpServer tcp_server_;
};
