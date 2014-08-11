#include "azer/render/gpu_constants_table.h"
#include "azer/math/math.h"
#include "base/basictypes.h"
#include "base/logging.h"
#include "gtest/gtest.h"

using namespace azer;

class DummyGpuConstantsTable : azer::GpuConstantsTable {
 public:
  virtual void flush() {
  }
};

TEST(GpuConstantsTable, SetValue) {
}

TEST(GpuConstantsTable, SetLight) {
}

