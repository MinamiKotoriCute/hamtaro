#pragma once

#include <boost/asio.hpp>

template<typename T>
using awaitable = boost::asio::awaitable<T>;
