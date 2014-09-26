#include "azer/afx/afxc/flags.h"

#include <iostream>

#include "base/command_line.h"
#include "base/logging.h"

bool FLAGS_hlslang = false;
bool FLAGS_glslang = false;
base::FilePath::StringType FLAGS_afxpath;
base::FilePath::StringType FLAGS_includes;
std::string FLAGS_output_dir;
std::string FLAGS_cpp_filename;

void PrintHelp() {
  std::cout << "afxc: transloate afx to hlsl or glslang, and generate "
            << " c++ wrapper class " << std::endl
            << "  --afx" << std::endl
            << "  --output_dir" << std::endl
            << "  --includes" << std::endl
            << "  --glslang: generate glslang code" << std::endl
            << "  --hlslang: generate hlslang code" << std::endl;
}

int ParseArgs() {
  CommandLine* cmd = CommandLine::ForCurrentProcess();
  if (cmd->HasSwitch("help")) {
    PrintHelp();
    return -1;
  }

  FLAGS_afxpath = cmd->GetSwitchValueNative("afx");
  FLAGS_output_dir = cmd->GetSwitchValueASCII("output_dir");
  FLAGS_includes = cmd->GetSwitchValueNative("includes");
  FLAGS_hlslang = cmd->HasSwitch("hlslang");
  FLAGS_glslang = cmd->HasSwitch("glslang");
  FLAGS_cpp_filename = cmd->GetSwitchValueASCII("cpp_filename");

  if (FLAGS_includes.empty()) {
    std::cerr << "includes cannot be empty\n";
    return -1;
  }

  if (FLAGS_afxpath.empty()) {
    std::cerr << "afx cannot be empty\n";
    return -1;
  }

  if (FLAGS_output_dir.empty()) {
    std::cerr << "output_dir cannot be empty\n";
    return -1;
  }

  if (FLAGS_cpp_filename.empty()) {
    std::cerr << "cpp_filename cannot be empty\n";
    return -1;
  }

  return 0;
}
