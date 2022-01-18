#pragma once

#include <sstream>
#include <cstdio>
#include <memory>

#include <glog/logging.h>

#define RESULT_ERROR(...) Result<void>(ResultFailureType{}, ##__VA_ARGS__)
#define RESULT_SUCCESS Result<void>()

#define RESULT_CHECK(e) \
{ \
    auto &&r = e; \
    if (r.has_error()) \
    { \
        LOG(INFO) << r.unique_error_id(); \
        return r; \
    } \
}

#define RESULT_TRY(r, e) \
r = e; \
if (r.has_error()) \
{ \
    LOG(INFO) << r.unique_error_id(); \
    return r; \
} \

#define RESULT_AUTO(r, e) RESULT_TRY(auto &&r, e)

#define RESULT_CO_CHECK(e) \
{ \
    auto &&r = e; \
    if (r.has_error()) \
    { \
        LOG(INFO) << r.unique_error_id(); \
        co_return r; \
    } \
}

#define RESULT_CO_TRY(r, e) \
r = e; \
if (r.has_error()) \
{ \
    LOG(INFO) << r.unique_error_id(); \
    co_return r; \
} \

#define RESULT_CO_AUTO(r, e) RESULT_CO_TRY(auto &&r, e)


int64_t result_get_unique_error_id();
std::string string_format(const std::string format, ...);
std::string string_format();

struct ResultFailureType
{
};

template<typename ValueType = void>
class Result;

template<>
class Result<void>
{
    using this_type = Result;

public:
    Result() :
        unique_error_id_(0)
    {
    }

    template<typename... Args>
    Result(ResultFailureType, Args&&... args) :
        unique_error_id_(result_get_unique_error_id()),
        error_(string_format(std::forward<Args>(args)...))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        unique_error_id_(other.unique_error_id_),
        error_(other.error_)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        unique_error_id_(std::forward<U>(other).unique_error_id_),
        error_(std::forward<U>(other).error_)
    {
    }

    template<typename U>
    this_type& operator<<(const U &other)
    {
        std::stringstream ss;
        ss << other;
        error_ += ss.str();
        return *this;
    }

    operator bool() const
    {
        return has_error();
    }

    bool has_error() const
    {
        return unique_error_id_ != 0;
    }

    std::string error_message() const
    {
        return error_;
    }

    int64_t unique_error_id() const
    {
        return unique_error_id_;
    }

protected:
    int64_t unique_error_id_;
    std::string error_;

private:
    void print()
    {
    }
};

template<typename ValueType>
class Result
{
    using this_type = Result;

public:
    Result() :
        unique_error_id_(0)
    {
    }

    Result(ResultFailureType) :
        unique_error_id_(result_get_unique_error_id())
    {
    }

    Result(const ValueType &x) :
        unique_error_id_(0),
        value_(x)
    {
    }

    Result(ValueType &&x) :
        unique_error_id_(0),
        value_(std::forward<ValueType>(x))
    {
    }

    template<typename U>
    Result(const Result<U> &other) :
        unique_error_id_(other.unique_error_id_),
        error_(other.error_),
        value_(other.value_)
    {
    }

    template<typename U>
    Result(Result<U> &&other) :
        unique_error_id_(std::forward<U>(other).unique_error_id_),
        error_(std::forward<U>(other).error_),        
        value_(std::forward<U>(other).value_)
    {
    }

    template<typename U>
    this_type& operator<<(const U &other)
    {
        std::stringstream ss;
        ss << other;
        error_ += ss.str();
        return *this;
    }

    operator bool() const
    {
        return has_error();
    }

    bool has_error() const
    {
        return unique_error_id_ != 0;
    }

    std::string error_message() const
    {
        return error_;
    }

    int64_t unique_error_id() const
    {
        return unique_error_id_;
    }

protected:
    int64_t unique_error_id_;
    std::string error_;
    ValueType value_;
};
