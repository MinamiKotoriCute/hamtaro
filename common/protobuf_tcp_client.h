#pragma once

#include <map>
#include <google/protobuf/message.h>

#include "awaitable.h"
#include "error_handle.h"
#include "tcp_client.h"

template<typename PackCoder, typename PackTcpReader>
class basic_ProtobufTcpClient
{
    using ReadResultType = result<std::unique_ptr<google::protobuf::Message>>;
    using ReadCallbackType = std::function<void(ReadResultType&&)>;

    //using PackVersionType = typename PackCoder::PackVersionType;

public:
    using ConnectResultType = TcpClient::ConnectResultType;
    using ConnectCallbackType = std::function<void(ConnectResultType&&)>;
    using SendResultType = TcpClient::WriteResultType;
    using SendCallbackType = std::function<void(SendResultType&&)>;
    using WaitCallbackType = std::function<void(boost::system::error_code, std::shared_ptr<google::protobuf::Message>)>;
    using MessageResultType = const google::protobuf::Message &;
    using MessageCallbackType = std::function<void(const MessageResultType&)>;
    template<typename T>
    using RealMessageCallbackType = std::function<void(const T&)>;
    using MessageCoroutineCallbackType = std::function<result<void>(const MessageResultType&)>;
    template<typename T>
    using RealMessageCoroutineCallbackType = std::function<result<void>(const T&)>;

    basic_ProtobufTcpClient(boost::asio::io_context& io_context) :
        tcp_client_(io_context),
        pack_tcp_reader_(tcp_client_),
        current_version_(0),
        is_receiving_(false)        
    {
    }

    basic_ProtobufTcpClient(boost::asio::strand<boost::asio::io_context::executor_type>& strand) :
        tcp_client_(strand),
        pack_tcp_reader_(tcp_client_),
        current_version_(0),
        is_receiving_(false)
    {
    }

    basic_ProtobufTcpClient(boost::asio::ip::tcp::socket socket) :
        tcp_client_(std::move(socket)),
        pack_tcp_reader_(tcp_client_),
        current_version_(0),
        is_receiving_(false)
    {
    }

    basic_ProtobufTcpClient(const basic_ProtobufTcpClient &) = delete;
    basic_ProtobufTcpClient(basic_ProtobufTcpClient&&) = default;

	awaitable<ConnectResultType> connect(const std::string& url)
    {
        return tcp_client_.connect(url);
    }

    awaitable<result<void>> start_receive()
    {
        is_receiving_ = true;
        while (is_receiving_)
        {
            RESULT_CO_AUTO(buffer, co_await pack_tcp_reader_.read());

            RESULT_CO_AUTO(message, pack_coder_.decode(std::move(buffer)));

            const auto &pb_name = message->GetDescriptor()->full_name();

            // handle
            {
                auto it = wait_callback_group_.find(pb_name);
                if (it != wait_callback_group_.end())
                {
                    auto callback = std::move(it->second);
                    wait_callback_group_.erase(it);
                    callback(boost::system::error_code{}, message);
                    continue;
                }
            }

            // {
            //     auto it = message_callback_group_.find(pb_name);
            //     if (it != message_callback_group_.end()) {
            //         auto &callback = it->second;
            //         callback(message);
            //         continue;
            //     }
            // }
        }

        is_receiving_ = false;

        co_return RESULT_SUCCESS;
    }

    void close()
    {
        tcp_client_.disconnect();
    }

    awaitable<SendResultType> send(const google::protobuf::Message &message)
    {
        PackCoder pack;
        auto write_buffer = pack.encode(message);
        if (write_buffer.empty())
        {
            auto error_result = RESULT_ERROR("pack encode fail.");
            LOG(WARNING) << error_result;
            co_return error_result;
        }

        co_return co_await tcp_client_.write(std::move(write_buffer));
    }

    template<typename T>
    awaitable<result<std::shared_ptr<T>>> send(const google::protobuf::Message &message)
    {
        RESULT_CO_CHECK(co_await send(message));
        co_return co_await wait<T>();
    }

    template<typename T>
    awaitable<result<std::shared_ptr<T>>> wait()
    {
        // TODO: fix two way wait same protobuf.

        // boost::system::error_code ec;
        // auto result = co_await wait<T>(boost::asio::redirect_error(boost::asio::use_awaitable, ec));

        // if (!ec)
        // { 
        //     co_return RESULT_ERROR("wait fail. error_message:") << ec.what();
        // }

        auto result = co_await wait<T>(boost::asio::use_awaitable);
        
        co_return std::dynamic_pointer_cast<T>(result);
    }

    void add_message_callback(const std::string &pb_name, MessageCallbackType &&callback)
    {        
        if (message_callback_group_.find(pb_name) != message_callback_group_.end())
        {
            LOG(ERROR) << "add duplicate message callback pbname:" << pb_name;
        }
        message_callback_group_[pb_name] = std::forward<MessageCallbackType>(callback);
    }
    void add_message_callback(const std::string &pb_name, MessageCoroutineCallbackType &&callback)
    {        
        if (message_coroutine_callback_group_.find(pb_name) != message_coroutine_callback_group_.end())
        {
            LOG(ERROR) << "add duplicate message coroutine callback pbname:" << pb_name;
        }
        message_coroutine_callback_group_[pb_name] = std::forward<MessageCoroutineCallbackType>(callback);
    }
    template<typename T>
    struct message_callback_lambda_helper
    {
        using type = void;
    };
    template<typename Ret, typename Class, typename Args>
    struct message_callback_lambda_helper<Ret (Class::*)(Args) const>
    {
        using type = Args;
    };
    template<typename Ret, typename Class, typename Args>
    struct message_callback_lambda_helper<Ret (Class::*)(Args)>
    {
        using type = Args;
    };
    template<typename Callback>
    void add_message_callback(Callback &&callback)
    {
        using Arg = typename message_callback_lambda_helper<decltype(&Callback::operator())>::type;
        using T = std::remove_reference_t<std::remove_const_t<Arg>>;
        add_message_callback(T::descriptor()->full_name(), [callback = std::forward<Callback>(callback)] (MessageResultType &&result)
        {
            callback(dynamic_cast<const T&>(result));
        });
    }
    template<typename Class, typename Function>
    void add_message_callback(Class *ptr, Function &&f)
    {
        using T = std::remove_reference_t<typename message_callback_lambda_helper<Function>::type>::element_type;
        add_message_callback(T::descriptor()->full_name(), [ptr, f = std::forward<Function>(f)] (MessageResultType &&result)
        {
            (ptr->*f)(std::unique_ptr<T>(dynamic_cast<const T&>(result)));
        });
    }

private:
    template<typename T, typename Handler>
    auto wait(Handler &&handler)
    {
        return boost::asio::async_initiate<Handler, void(boost::system::error_code, std::shared_ptr<google::protobuf::Message>)>
            (
                [this]<typename H> (H&& self) mutable
                {
                    wait_callback_group_.insert(std::make_pair(T::descriptor()->full_name(),
                        [self = std::make_shared<H>(std::forward<H>(self))] (boost::system::error_code ec, std::shared_ptr<google::protobuf::Message> result) mutable
                        {
                            (*self)(ec, result);
                        }));
                },
                std::forward<Handler>(handler)
            );
    }

    TcpClient tcp_client_;
    PackTcpReader pack_tcp_reader_;
    PackCoder pack_coder_;
    int current_version_;
    std::map<std::string, WaitCallbackType> wait_callback_group_;
    std::map<std::string, MessageCallbackType> message_callback_group_;
    std::map<std::string, MessageCoroutineCallbackType> message_coroutine_callback_group_;
    bool is_receiving_;
};
