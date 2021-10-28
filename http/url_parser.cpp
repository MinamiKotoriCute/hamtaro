#include "url_parser.h"

#include <regex>
#include <iostream>

#include <glog/logging.h>

#include "error/error_define.h"


boost::leaf::result<void> url_parser::parse(const std::string &url)
{
    auto load = boost::leaf::on_error([&](ErrorInfo &error_info)
    {
        error_info.value["url"] = url;
    });

    std::regex regex(R"##((?:(\w+):\/\/)?([^:\/]+):?(\d+)?(\/.*)?)##");
    std::smatch smatch;
    if (!std::regex_match(url, smatch, regex))
    {
        return NEW_ERROR(InputError{"regex match fail."});
    }

    if (smatch.size() != 5)
    {
        return NEW_ERROR(InputError{"regex match size fail."});
    }

    protocol_ = smatch[1].str();
    host_ = smatch[2].str();
    port_ = smatch[3].str();
    path_ = smatch[4].str();

    return {};
}

std::string_view url_parser::path() const noexcept
{
    if (!path_.empty())
        return path_;
    return "/";
}

std::string_view url_parser::service() const noexcept
{
    if (!protocol_.empty())
        return protocol_;
    else if (!port_.empty())
        return port_;
    return "80";
}
