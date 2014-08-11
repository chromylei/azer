#include "base/base.h"

int main(int argc, char* argv[]) {
  base::InitApp(&argc, &argv, "");
  DCHECK(false) << "dcheck";
  LOG(INFO) << "Hello, world";
  LOG(WARNING) << "Hello, world";
  LOG(ERROR) << "Hello, world";
  LOG(FATAL) << "Hello, world";
  DLOG(INFO) << "Hello, world";
  DLOG(WARNING) << "Hello, world";
  DLOG(ERROR) << "Hello, world";
  DLOG(FATAL) << "Hello, world";
  return 0;
}
