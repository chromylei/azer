#include "azer/afx/linker/afx_parser.h"

#include "base/file_util.h"
#include "base/files/file_path.h"

namespace azer {
namespace afx {
bool AfxParser::Parse(const ::base::FilePath& path) {
  std::string content;
  DCHECK(initializer_ == false);
  if (!::base::ReadFileToString(path, &content)) {
    return false;
  }

  return Parse(content, path);
}

bool AfxParser::Parse(const std::string& content, const ::base::FilePath& path) {
  initializer_ = true;
  if (!linker_.Load(content, path)) {
    return false;
  }

  ParseContext* context = linker_.root()->GetContext();
  if (!tparser_.Parse(context)) {
    return false;
  }

  return true;
}

TechniqueParser::Technique* AfxParser::GetTechnique(const std::string& name) {
  TechniqueParser::Technique* tech = NULL;
  if (tparser_.GetTechnique(name, &tech)) {
    return tech;
  } else {
    return NULL;
  }
}

ParseContext* AfxParser::GetContext() {
  DCHECK(linker_.root() != NULL);
  return linker_.root()->GetContext();
}
}  // namespace afx
}  // namespace azer
