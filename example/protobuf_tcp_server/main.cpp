#include <iostream>

#include <boost/asio/experimental/awaitable_operators.hpp>

#include "common/pack_coder.h"
#include "common/pack_tcp_reader.h"
#include "common/protobuf_tcp_client.h"
#include "common/protobuf_tcp_server.h"

#include "pb/pb2.pb.h"
#include "pb/pb4.pb.h"
#include "pb/game.pb.h"

int port = 0;

boost::asio::awaitable<result<void>> g(basic_ProtobufTcpClient<PackCoder, PackTcpReader> &protobuf_tcp_client)
{
    // protobuf_tcp_client.add_message_callback([&protobuf_tcp_client]( $req ) -> awaitable<result<void>>
    // {
    //     // ... handle ...
    //     co_return RESULT_SUCCESS;
    // });

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<result<void>> f(boost::asio::io_context &io_context)
{
    basic_ProtobufTcpServer<PackCoder, PackTcpReader> protobuf_tcp_server(io_context);

    RESULT_CO_CHECK(protobuf_tcp_server.listen("192.168.1.221", port));

    while (true)
    {
        RESULT_CO_AUTO(protobuf_tcp_client, co_await protobuf_tcp_server.accept());
        LOG(INFO) << "new client";

        boost::asio::co_spawn(io_context,
            [protobuf_tcp_client = std::move(protobuf_tcp_client)]() -> awaitable<result<void>>
            {            
                using namespace boost::asio::experimental::awaitable_operators;
                std::tuple<result<void>, result<void>> results = co_await ( g(*protobuf_tcp_client.get()) && protobuf_tcp_client->start_receive() );

                RESULT_CO_CHECK(std::get<0>(results));
                RESULT_CO_CHECK(std::get<1>(results));
                
                co_return RESULT_SUCCESS;
            },
            [](std::exception_ptr e, result<void> result)
            {
                if (result.has_error())
                {
                    LOG(INFO) << "unique_error_id:" << result.unique_error_id() << " error_message:" << result.error_message() << "\n";
                }
            });
    }

    co_return RESULT_SUCCESS;
}

// ncat -l 2000 -k -c 'xargs -n1 echo'
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        LOG(INFO) << argv[0] << " ${port}";
        return -1;
    }
    port = std::atoi(argv[1]);

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