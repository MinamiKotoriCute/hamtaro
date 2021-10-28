#pragma once
#include <functional>
#include <optional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/leaf.hpp>

#include <glog/logging.h>

#include "error/error_define.h"
#include "url_parser.h"

template<typename Executor = boost::asio::any_io_executor>
class HttpClient
{
public:
    using ResponseResultType = boost::leaf::result<std::string>;
    using ResponseCallbackType = std::function<void(ResponseResultType&&)>;
    using ConnectResultType = std::optional<boost::asio::ip::tcp::endpoint>;
    using ConnectCallbackType = std::function<void(ConnectResultType&&)>;

    HttpClient(Executor &ex) :
        executor_(ex),
        resolver_(ex),
        stream_(ex)
    {
    }

    ResponseResultType get(const std::string &url)
    {
        auto load = boost::leaf::on_error([&](ErrorInfo &error_info)
        {
            error_info.value["url"] = url;
        });

        // domain resolve
        BOOST_LEAF_CHECK(parser_.parse(url));

        boost::system::error_code ec;
        auto results = resolver_.resolve(parser_.host(), parser_.service(), ec);
        if (ec)
        {
            return NEW_ERROR(HandleError{ec.message()});
        }

        if (results.empty())
        {
            return NEW_ERROR(HandleError{"can't find ip from domain."});
        }

        // tcp connect
        stream_.connect(results, ec);
        if (ec)
        {
            return NEW_ERROR(HandleError{ec.message()});
        }

        // http requent
        //boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, std::string(parser.path()), 11};
        req_.set(boost::beast::http::field::host, std::string(parser_.host()));
        req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        boost::beast::http::write(stream_, req_, ec);
        if (ec)
        {
            return NEW_ERROR(HandleError{ec.message()});
        }

        // http response
        boost::beast::http::read(stream_, rsp_buffer_, rsp_, ec);
        if (ec)
        {
            return NEW_ERROR(HandleError{ec.message()});
        }

        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec && ec != boost::beast::errc::not_connected)
        {
            return NEW_ERROR(HandleError{ec.message()});
        }

        return rsp_.body();
    }

    boost::leaf::result<void> async_get(const std::string &url, ResponseCallbackType &&callback)
    {
        // domain resolve
        BOOST_LEAF_CHECK(parser_.parse(url));

        resolver_.async_resolve(parser_.host(), parser_.service(), [this, callback = std::forward<ResponseCallbackType>(callback)](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) mutable
        {
            if (ec)
            {
                callback(NEW_ERROR(HandleError{ec.message()}));
                return;
            }

            if (results.empty())
            {
                callback(NEW_ERROR(HandleError{"can't find ip from domain."}));
                return;
            }

            stream_.async_connect(results, [this, callback = std::forward<ResponseCallbackType>(callback)](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) mutable
            {
                if (ec)
                {
                    callback(NEW_ERROR(HandleError{ec.message()}));
                    return;
                }

                req_.set(boost::beast::http::field::host, std::string(parser_.host()));
                req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                boost::beast::http::async_write(stream_, req_, [this, callback = std::forward<ResponseCallbackType>(callback)](const boost::system::error_code& ec, std::size_t bytes_transferred) mutable
                {
                    if (ec)
                    {
                        callback(NEW_ERROR(HandleError{ec.message()}));
                        return;
                    }

                    boost::beast::http::async_read(stream_, rsp_buffer_, rsp_, [this, callback = std::forward<ResponseCallbackType>(callback)](const boost::system::error_code& ec, std::size_t bytes_transferred)
                    {
                        if (ec)
                        {
                            callback(NEW_ERROR(HandleError{ec.message()}));
                            return;
                        }

                        boost::system::error_code ec2;
                        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec2);
                        if (ec2 && ec2 != boost::beast::errc::not_connected)
                        {
                            callback(NEW_ERROR(HandleError{ec2.message()}));
                            return;
                        }

                        callback(rsp_.body());
                    });
                });
            });
        });        
    }

private:
    Executor& executor_;
    url_parser parser_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    boost::beast::flat_buffer rsp_buffer_;
    boost::beast::http::response<boost::beast::http::string_body> rsp_;
};
