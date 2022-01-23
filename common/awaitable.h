#pragma once

#include <type_traits>
#include <boost/asio.hpp>

template<typename T>
using awaitable = boost::asio::awaitable<T>;

template<typename T>
struct is_awaitable : public std::false_type
{
};

template<typename T>
struct is_awaitable<awaitable<T>> : public std::true_type
{
};

template<typename T>
inline constexpr bool is_awaitable_v = is_awaitable<T>::value;
