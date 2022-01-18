#include "url_parser.h"

#include <regex>
#include <iostream>

#include <glog/logging.h>

result<void> UrlParser::parse(const std::string &url)
{
    std::regex regex(R"##((?:(\w+):\/\/)?([^:\/]+):?(\d+)?(\/.*)?)##");
    std::smatch smatch;
    if (!std::regex_match(url, smatch, regex))
    {
        LOG(INFO) << "regex match fail. url:" << url;
        return RESULT_ERROR() << "regex match fail. url:" << url;
    }

    if (smatch.size() != 5)
    {
        LOG(INFO) << "regex match size fail. url:" << url;
        return RESULT_ERROR() << "regex match size fail. url:" << url;
    }

    protocol_ = smatch[1].str();
    host_ = smatch[2].str();
    port_ = smatch[3].str();
    path_ = smatch[4].str();

    return RESULT_SUCCESS;
}

std::string_view UrlParser::path() const noexcept
{
    if (!path_.empty())
        return path_;
    return "/";
}

std::string UrlParser::service() const noexcept
{
    if (!protocol_.empty())
        return protocol_;
    else if (!port_.empty())
        return port_;
    return "80";
}
