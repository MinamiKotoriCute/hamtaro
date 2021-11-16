#include <iostream>

#include "http/http_client.h"

boost::asio::awaitable<boost::leaf::result<void>> co_g(boost::asio::io_context &io_context)
{
    HttpClient http_client(io_context);
    CO_LEAF_AUTO(rsp, co_await http_client.co_get("http://myip.com.tw/"));
    std::cout << "co_get:" << rsp;
}

void f()
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
        std::bind(co_g, std::ref(io_context)),
        [](std::exception_ptr e, boost::leaf::result<void> r)
        {
            boost::leaf::try_handle_all(
                [&r]() -> boost::leaf::result<void>
                {
                    BOOST_LEAF_CHECK(r);
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
        });

    io_context.run();

}

int main(int argc, char *argv[])
{
    f();

    return 0;
}