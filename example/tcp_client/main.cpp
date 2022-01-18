#include <iostream>

#include "common/tcp_client.h"

boost::asio::awaitable<result<void>> f(boost::asio::io_context &io_context)
{
    TcpClient tcp_client(io_context);
    RESULT_CO_CHECK(co_await tcp_client.connect("192.168.1.216:2000"));
    
    while (true)
    {
        std::cout << "enter:";

        std::string input;
        std::cin >> input;
        input += '\n';
        RESULT_CO_CHECK(co_await tcp_client.write({&input[0], &input[0]+input.size()}));

        std::vector<char> read_buffer;
        read_buffer.resize(input.size());
        RESULT_CO_CHECK(co_await tcp_client.read(read_buffer));

        std::string receive(&read_buffer[0], read_buffer.size());
        std::cout << "receive:" << receive << std::endl;
    }
}

// ncat -l 2000 -k -c 'xargs -n1 echo'
int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;

    boost::asio::co_spawn(io_context,
    f(io_context),
    [](std::exception_ptr e, result<void> result)
    {
        if (result.has_error())
        {
            LOG(INFO) << "unique_error_id:" << result.unique_error_id() << " error_message:" << result.error_message() << "\n";
        }
    });

    io_context.run();

    return 0;
}