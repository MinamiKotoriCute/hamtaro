#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/leaf.hpp>

#include "error/error_define.h"
#include "url_parser.h"

template<typename Executor = boost::asio::any_io_executor>
class HttpServer
{
public:
    using RegistCallbackType = std::function<boost::leaf::result<std::string>(std::string_view body)>;

    HttpServer(Executor &ex) :
        executor_(ex)
    {
    }

    void regist(const std::string &path, RegistCallbackType &&callback)
    {
        callbacks_[path] = std::forward<RegistCallbackType>(callback);
    }

    boost::leaf::result<void> listen(const std::string &url)
    {
        auto load = boost::leaf::on_error([&](ErrorInfo &error_info)
        {
            error_info.value["url"] = url;
        });

        // domain resolve
        BOOST_LEAF_CHECK(parser_.parse(url));

        const auto address = boost::asio::ip::make_address(parser_.host());
        const auto port = static_cast<unsigned short>(std::atoi(parser_.port().data()));

        boost::beast::flat_buffer buffer;
        boost::beast::error_code ec;
        boost::asio::ip::tcp::acceptor acceptor{executor_, {address, port}};
        for(;;)
        {
            // This will receive the new connection
            boost::asio::ip::tcp::socket socket{executor_};

            // Block until we get a connection
            acceptor.accept(socket);
            LOG(INFO) << "AAA";

            boost::beast::http::request<boost::beast::http::string_body> req;
            boost::beast::http::read(socket, buffer, req, ec);
            if (ec == boost::beast::http::error::end_of_stream)
                break;
            if (ec)
                return NEW_ERROR(HandleError{"read"});
            
            LOG(INFO) << "AAA " << req.target();
            std::string rsp_body;
            auto it = callbacks_.find(std::string(req.target()));
            if (it == callbacks_.end())
            {
                rsp_body = "path not find";
            }
            else
            {
                BOOST_LEAF_ASSIGN(rsp_body, it->second(req.body()));
            }

            boost::beast::http::response<boost::beast::http::string_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(rsp_body)),
                std::make_tuple(boost::beast::http::status::ok, req.version())};
            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, "application/text");
            res.content_length(rsp_body.size());
            boost::beast::http::serializer<false, decltype(res)::body_type, decltype(res)::fields_type> sr{res};
            boost::beast::http::write(socket, sr, ec);
            if (ec)
                return NEW_ERROR(HandleError{"write"});
            
            socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        }

        return {};
    }

private:
    Executor& executor_;
    url_parser parser_;
    std::map<std::string, RegistCallbackType> callbacks_;
};
