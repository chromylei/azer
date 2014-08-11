#include "gtest/gtest.h"
#include "base/command_line.h"

#include "base/at_exit.h"
#include "base/files/file_path.h"
#include "base/i18n/icu_util.h"

int main(int argc, char* argv[]) {
  static ::base::AtExitManager at_exit;
  testing::InitGoogleTest(&argc, argv);
  CommandLine::Init(argc, argv);

  ::logging::LoggingSettings setting;
  setting.log_file = TEXT("azer.log");
  ::logging::InitLogging(setting);

  base::i18n::InitializeICU();
  return RUN_ALL_TESTS();
}

