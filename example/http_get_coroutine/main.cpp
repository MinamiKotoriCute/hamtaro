#include <iostream>

#include "http/http_client.h"

void f()
{
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void>
        {
            boost::asio::io_context io_context;

            boost::asio::co_spawn(io_context,
                [&io_context]() -> boost::asio::awaitable<void>
                {
                    HttpClient http_client(io_context);
                    auto r = co_await http_client.co_get("http://myip.com.tw/");
                    if (r)
                    {
                        std::cout << "co_get: " << r.value() << std::endl;
                    }
                },
                boost::asio::detached);

            io_context.run();
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