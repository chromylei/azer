#pragma once

#include <string>
#include "base/files/file_path.h"

extern bool FLAGS_hlslang;
extern bool FLAGS_glslang;
extern base::FilePath::StringType FLAGS_afxpath;
extern base::FilePath::StringType FLAGS_includes;
extern std::string FLAGS_output_dir;
extern std::string FLAGS_cpp_filename;

int ParseArgs();
void PrintHelp();
