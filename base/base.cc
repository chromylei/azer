#include "base/base.h"

#include "base/command_line.h"
#include "base/at_exit.h"
#include "base/logging.h"
#include "base/i18n/icu_util.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"

namespace base {

void InitApp(int* argc, char** argv[], const char*) {
  static ::base::AtExitManager at_exit;
  CommandLine::Init(*argc, *argv);
  base::FilePath path = CommandLine::ForCurrentProcess()->GetProgram();

  ::logging::LoggingSettings setting;
  // setting.log_file = UTF16ToUTF8(path.value());
  setting.log_file = TEXT("azer.log");
  ::logging::InitLogging(setting);

  base::i18n::InitializeICU();
}
}  // namespace base
