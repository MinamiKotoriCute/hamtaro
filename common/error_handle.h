#pragma once

#if 0
#include <boost/outcome.hpp>

namespace outcome = boost::outcome_v2;

template<typename T>
using result = outcome::result<T>;
#endif


#include "result.h"
template<typename T>
using result = Result<T>;
