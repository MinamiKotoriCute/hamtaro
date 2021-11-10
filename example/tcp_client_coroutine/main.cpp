#include <iostream>

#include <glog/logging.h>

#include "http/tcp_client.h"

void f()
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
        [&io_context]() -> boost::asio::awaitable<boost::leaf::result<void>>
        {
            LOG(INFO) << "AA";
            TcpClient tcp_client(io_context);
            CO_LEAF_CHECK(co_await tcp_client.co_connect("192.168.1.216:2000"));
            LOG(INFO) << "AA";
            CO_LEAF_CHECK(co_await tcp_client.co_write({'a', 'a', '1', '\0'}));
            LOG(INFO) << "AA";

            std::vector<char> read_buffer;
            CO_LEAF_CHECK(co_await tcp_client.co_read(read_buffer));
            LOG(INFO) << "AA";
            for (const auto &c : read_buffer)
            {
                std::cout << c;
            }
            std::cout << std::endl;
        },
        [](std::exception_ptr e, boost::leaf::result<void> r)
        {
            LOG(INFO) << "AA";
        });

    LOG(INFO) << "AA";
    //io_context.run();
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work = boost::asio::make_work_guard(io_context);
    LOG(INFO) << "AA";


    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void>
        {
            return {};
        },
        [](const InputError &error, const ErrorInfo &error_info, const boost::leaf::e_source_location &source_location)
        {
            std::cout << "catch InputError. source_location:" << source_location << " error:" << error.value << " error_info:" << error_info << std::endl;
            return;
        },
        [](const HandleError &error, const ErrorInfo &error_info, const boost::leaf::e_source_location &source_location)
        {
            std::cout << "catch HandleError. source_location:" << source_location << "error:" << error.value << " error_info:" << error_info << std::endl;
            return;
        },
        [](const boost::leaf::error_info &unmatched)
        { 
            std::cerr <<
                "Unknown failure detected" << std::endl <<
                "Cryptic diagnostic information follows" << std::endl <<
                unmatched;
            return;
        }
    );
}

int main(int argc, char *argv[])
{
    f();

    return 0;
}