#include <iostream>

#include "http/http_client.h"

void f()
{
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void>
        {
            boost::asio::io_context io_context;
            HttpClient http_client(io_context);
            BOOST_LEAF_AUTO(res, http_client.get("http://myip.com.tw/"));
            std::cout << res << std::endl;
            HttpClient http_client2(io_context);
            BOOST_LEAF_CHECK(http_client2.async_get("http://myip.com.tw/", [](boost::leaf::result<std::string> &&r)
            {
                if (r)
                {
                    std::cout << "good" << std::endl;
                    std::cout << r.value() << std::endl;
                }
                else
                {
                    std::cout << "gg" << std::endl;
                }
            }));

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