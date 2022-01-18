#pragma once

#include <string>
#include <string_view>

#include "error_handle.h"

class UrlParser
{
public:
    result<void> parse(const std::string &url);

    std::string_view protocol() const noexcept { return protocol_; }
    std::string host() const noexcept { return host_; }
    std::string_view port() const noexcept { return port_; }
    std::string_view path() const noexcept;
    std::string service() const noexcept;

private:
    std::string protocol_;
    std::string host_;
    std::string port_;
    std::string path_;
};
