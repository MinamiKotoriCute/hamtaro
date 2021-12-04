#include <iostream>
#include <glog/logging.h>
#include "http/http_server.h"


void f()
{
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void>
        {
            boost::asio::io_context io_context;

            HttpServer http_server(io_context);
            http_server.regist("/", [](std::string_view body) -> std::string
            {
                LOG(INFO) << body;
                return "gg";
            });
            BOOST_LEAF_CHECK(http_server.listen("0.0.0.0:44555"));

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