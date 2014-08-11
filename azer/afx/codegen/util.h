#pragma once

#include <string>
#include "base/files/file_path.h"

namespace azer {
namespace afx {

class ASTNode;
std::string FormatCode(const std::string& code);
}  // namespace afx
}  // namespace azer

bool CompareCode(const ::base::FilePath& path, const std::string& content);
