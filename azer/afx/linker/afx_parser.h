#pragma once

#include <string>
#include <vector>
#include "azer/afx/codegen/afx_codegen.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/linker/technique_parser.h"
#include "azer/afx/linker/linker.h"
#include "azer/afx/linker/testutil.h"
#include "azer/afx/util/file_loader.h"
#include "azer/render/render_system_enum.h"
#include "base/basictypes.h"
#include "base/logging.h"

namespace azer {
namespace afx {
class AfxParser {
 public:
  AfxParser(const std::vector< ::base::FilePath::StringType>& incpath,
            const AfxLinker::Options& link_opt)
      : initializer_(false)
      , loader_(incpath)
      , linker_(&loader_, link_opt) {
  }

  bool Parse(const ::base::FilePath& path);
  bool Parse(const ::base::FilePath::StringType& path);
  bool Parse(const std::string& content, const ::base::FilePath& path);
  TechniqueParser::Technique* GetTechnique(const std::string& name);
  TechniqueParser* GetTechniques() { return &tparser_;}

  ParseContext* GetContext();

  bool success() const {
    DCHECK(initializer_);
    return linker_.success();
  }

  std::string GetCompileError() const {
    DCHECK(initializer_);
    return linker_.GetCompileError();
  }

  const std::string GetErrorText() const {
    DCHECK(initializer_);
    return linker_.GetErrorText();
  }
 private:
  bool initializer_;
  FileLoader loader_;
  AfxLinker linker_;
  TechniqueParser tparser_;
  DISALLOW_COPY_AND_ASSIGN(AfxParser);
};

inline bool AfxParser::Parse(const ::base::FilePath::StringType& path) {
  return Parse(::base::FilePath(path));
}

}  // namespace afx
}  // namespace azer
