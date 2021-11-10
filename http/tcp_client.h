#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <boost/leaf.hpp>

#include "error/error_define.h"
#include "url_parser.h"

template<typename Executor = boost::asio::any_io_executor>
class TcpClient
{
public:
    using ConnectResultType = boost::leaf::result<void>;
    using ConnectCallbackType = std::function<void(ConnectResultType&&)>;
    using WriteResultType = boost::leaf::result<void>;
    using WriteCallbackType = std::function<void(WriteResultType&&)>;
    using ReadResultType = boost::leaf::result<void>;
    using ReadCallbackType = std::function<void(ReadResultType&&)>;

    TcpClient(Executor& ex) :
        resolver_(ex),
        socket_(ex)
    {

    }
    
    boost::asio::awaitable<ConnectResultType> co_connect(const std::string &url)
    {
        CO_LEAF_CHECK(parser_.parse(url));

        try
        {
            LOG(INFO) << "AA";
            auto results = co_await resolver_.async_resolve(parser_.host(), parser_.service(), boost::asio::use_awaitable);
            LOG(INFO) << "AA";
            //co_await socket_.async_connect(results, boost::asio::use_awaitable);
            co_await boost::asio::async_connect(socket_, results, boost::asio::use_awaitable);
            LOG(INFO) << "AA";
        }
        catch(const boost::system::error_code &ec)
        {
            co_return NEW_ERROR(HandleError{ec.message()});
        }
    }

    boost::leaf::result<void> disconnect()
    {
        if (socket_.is_open()) {
            boost::system::error_code ec;
            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            if (ec)
            {
                return NEW_ERROR(HandleError{ec.message()});
            }

            socket_.close(ec);
            if (ec)
            {
                return NEW_ERROR(HandleError{ec.message()});
            }
        }

        return {};
    }

    boost::asio::awaitable<WriteResultType> co_write(std::vector<char> &&data)
    {
        auto write_buffer = std::make_shared<std::vector<char>>(std::forward<std::vector<char>>(data));
        co_await boost::asio::async_write(socket_, boost::asio::buffer(*write_buffer.get()), boost::asio::use_awaitable);
    }

    boost::asio::awaitable<ReadResultType> co_read(std::vector<char> &read_buffer)
    {
        co_await boost::asio::async_read(socket_, boost::asio::buffer(read_buffer), boost::asio::use_awaitable);
    }

private:
    url_parser parser_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	std::vector<char> write_buffer_;
};
