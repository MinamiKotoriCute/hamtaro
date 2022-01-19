#pragma once

#include <vector>
#include <functional>

#include "awaitable.h"
#include "error_handle.h"
#include "tcp_client.h"

class PackTcpReader
{
public:
    using ReadResultType = result<std::vector<char>>;
    using ReadCallbackType = std::function<void(ReadResultType&&)>;

    PackTcpReader(TcpClient &tcp_client);

    awaitable<ReadResultType> read();

private:
	std::vector<char> read_buffer_;
	TcpClient &tcp_client_;
};
