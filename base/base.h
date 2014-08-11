#pragma once

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/base_export.h"


#define ARRAY_SIZE(x) sizeof(x) / sizeof((x)[0])

namespace base {
void InitApp(int* argc, char** argv[], const char*);

}  // namespace base
