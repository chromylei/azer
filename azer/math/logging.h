#pragma once

#include <string>
#include <sstream>
#include "base/logging.h"
#include "azer/math/float_util.h"

namespace azer {

namespace detail {
inline std::string* MakeFloatEqString(float v1, float v2) {
  if (azer::Float(v1).AlmostEquals(Float(v2))) {
    return NULL;
  } else {
    std::ostringstream ss;
    ss << "CHECK_FLOAT_EQ(" << v1 << " vs. " << v2 << ")";
    return new std::string(ss.str());
  }
}

inline std::string* MakeDoubleEqString(double v1, double v2) {
  if (azer::Double(v1).AlmostEquals(Double(v2))) {
    return NULL;
  } else {
    std::ostringstream ss;
    ss << "CHECK_DOUBLE_EQ (" << v1 << " vs. " << v2 << ")";
    return new std::string(ss.str());
  }
}
}  // namespace detail
}  // namespace azer


/*
#if defined(DEBUG) || defined(_DEBUG)
#else
#endif
*/

#define CHECK_FLOAT_EQ(v1, v2)                                          \
  if (std::string* _result = azer::detail::MakeFloatEqString(v1, v2)) \
    logging::LogMessage(                                                \
        __FILE__, __LINE__, ::logging::LOG_DCHECK,                      \
        _result).stream()

#define DCHECK_FLOAT_EQ(v1, v2)                                         \
  if (DCHECK_IS_ON())                                                   \
    CHECK_FLOAT_EQ(v1, v2)

#define CHECK_DOUBLE_EQ(v1, v2)                                          \
  if (std::string* _result = azer::detail::MakeDoubleEqString(v1, v2)) \
    logging::LogMessage(                                                \
        __FILE__, __LINE__, ::logging::LOG_DCHECK,                      \
        _result).stream()

#define DCHECK_DOUBLE_EQ(v1, v2)                                         \
  if (DCHECK_IS_ON())                                                   \
    CHECK_FLOAT_EQ(v1, v2)

