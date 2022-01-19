#include <iostream>

#include <boost/asio/experimental/awaitable_operators.hpp>

#include "common/pack_coder.h"
#include "common/pack_tcp_reader.h"
#include "common/protobuf_tcp_client.h"

boost::asio::awaitable<result<void>> g(basic_ProtobufTcpClient<PackCoder, PackTcpReader> &protobuf_tcp_client)
{
    {
        // proto req declare

        // RESULT_CO_AUTO(rsp, co_await protobuf_tcp_client.send< $rsp_type >( $req ));
        // LOG(INFO) << rsp->ShortDebugString();
    }

    co_return RESULT_SUCCESS;
}

boost::asio::awaitable<result<void>> f(boost::asio::io_context &io_context)
{
    basic_ProtobufTcpClient<PackCoder, PackTcpReader> protobuf_tcp_client(io_context);
    RESULT_CO_CHECK(co_await protobuf_tcp_client.connect("192.168.1.216:4000"));

    using namespace boost::asio::experimental::awaitable_operators;
    co_await ( protobuf_tcp_client.start_receive() && g(protobuf_tcp_client));
    
    co_return RESULT_SUCCESS;
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