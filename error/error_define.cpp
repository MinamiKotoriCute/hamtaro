#include "error_define.h"


std::ostream& operator<<(std::ostream &os, const ErrorInfo &error_info)
{
    const auto &infos = error_info.value;
    if (infos.empty())
        return os;

    bool is_first = true;
    for (const auto &info : error_info.value)
    {
        if (is_first)
            is_first = false;
        else
            os << " ,";

        os << info.first << "=" << info.second;
    }

    return os;
}
