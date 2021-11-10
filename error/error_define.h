#pragma once

#include <string>
#include <ostream>
#include <map>

#include <boost/leaf.hpp>

#define NEW_ERROR boost BOOST_LEAF_NEW_ERROR
#define CO_LEAF_CHECK(r)\
    {\
        auto && BOOST_LEAF_TMP = r;\
        static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(BOOST_LEAF_TMP)>::type>::value, "CO_LEAF_CHECK requires a result type");\
        if( !BOOST_LEAF_TMP )\
            co_return BOOST_LEAF_TMP.error();\
    }

struct ErrorInfo
{
    std::map<std::string, std::string> value;

    friend std::ostream& operator<<(std::ostream &os, const ErrorInfo &error_info);
};

// 輸入的參數有問題，或是因此產生的問題
struct InputError
{
    std::string value; // 錯誤訊息
};

// 處理的過程有問題，或是相依的服務產生的問題
struct HandleError
{
    std::string value; // 錯誤訊息
};

