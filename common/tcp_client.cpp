#include "tcp_client.h"

#include <regex>
#include <glog/logging.h>
#include "url_parser.h"

TcpClient::TcpClient(boost::asio::io_context& io_context) :
    resolver_(io_context),
    socket_(io_context)
{

}

TcpClient::TcpClient(boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
    resolver_(strand),
    socket_(strand)
{

}

TcpClient::TcpClient(boost::asio::ip::tcp::socket socket) :
    resolver_(socket.get_executor()),
    socket_(std::move(socket))
{

}

boost::asio::awaitable<TcpClient::ConnectResultType> TcpClient::connect(const std::string &url)
{
    UrlParser url_parser;
    RESULT_CO_CHECK(url_parser.parse(url));

    boost::asio::ip::tcp::resolver::results_type ret_results;
	boost::asio::ip::tcp::resolver::query query(url_parser.host(), url_parser.service(), boost::asio::ip::resolver_query_base::numeric_service);

    boost::system::error_code ec;
	boost::asio::ip::tcp::resolver::results_type results = co_await resolver_.async_resolve(query, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        co_return RESULT_ERROR("url resolve fail. url:") << url << " error_message:" << ec.what();
    }

    boost::asio::ip::tcp::endpoint endpoint = co_await boost::asio::async_connect(socket_, results, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        co_return RESULT_ERROR("connect fail. url:") << url << " error_message:" << ec.what();
    }

    co_return RESULT_SUCCESS;
}

void TcpClient::disconnect()
{
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec)
        {
			LOG(WARNING) << ec.message();
		}

        socket_.close(ec);
        if (ec)
        {
			LOG(WARNING) << ec.message();
		}
    }
}

boost::asio::awaitable<TcpClient::WriteResultType> TcpClient::write(std::vector<char> &&data)
{
    boost::system::error_code ec;
    std::size_t bytes_transferred = co_await boost::asio::async_write(socket_, boost::asio::buffer(data), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        LOG(WARNING) << ec.message();
        co_return RESULT_ERROR("internal error");
    }

    if (bytes_transferred != data.size())
    {
        LOG(WARNING) << "write not finished. bytes_transferred:" << bytes_transferred << " write_buffer.size():" << data.size();
        co_return RESULT_ERROR("internal error");
    }

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<TcpClient::ReadResultType> TcpClient::read(std::vector<char> &read_buffer)
{
    boost::system::error_code ec;
    std::size_t bytes_transferred = co_await boost::asio::async_read(socket_, boost::asio::buffer(read_buffer), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec == boost::asio::error::eof)
    {
        // tcp closed
        co_return RESULT_ERROR("tcp closed");
    }
    else if (ec == boost::asio::error::bad_descriptor)
    {
        // tcp close by custom
        // call disconnect() will trigger this
        co_return RESULT_ERROR("tcp close by custom");
    }
    else if (ec == boost::asio::error::operation_aborted)
    {
        // tcp close by custom
        // call disconnect() will trigger this
        co_return RESULT_ERROR("tcp close by custom");
    }
    else if (ec == boost::asio::error::connection_reset)
    {
        // tcp close by remote
        co_return RESULT_ERROR("tcp close by remote");
    }
    else if (ec)
    {
        LOG(WARNING) << ec.message();
        co_return RESULT_ERROR() << ec.message();
    }

    co_return RESULT_SUCCESS;
}

boost::asio::any_io_executor TcpClient::get_executor()
{
    return socket_.get_executor();
}
