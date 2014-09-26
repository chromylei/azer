#include "azer/afx/afxc/afx_wrapper.h"

#include "azer/afx/codegen/afx_codegen.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/codegen/cpp_codegen.h"
#include "azer/afx/codegen/util.h"
#include "azer/base/string.h"
#include "base/strings/string_util.h"
#include "base/strings/string_split.h"
#include "base/strings/stringprintf.h"
#include "base/logging.h"

using ::base::FilePath;

namespace azer {
namespace afx {

AfxWrapper::AfxWrapper(const FilePath::StringType& includes) {
  ::base::SplitString(includes, FILE_PATH_LITERAL(','), &includes_);
}

bool AfxWrapper::Parse(const FilePath& path, std::ostream& os,
                       std::vector<AfxResult>* resvec) {
  DCHECK(parser_.get() == NULL);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  parser_.reset(new AfxParser(includes_, opt));
  parser_->Parse(path);
  if (!parser_->success()) {
    os << "Failed to compile afxfile: \"" << path.value() << "\"" << std::endl;
    if (!parser_->GetCompileError().empty()) {
      os << "compiler error: \n" << parser_->GetCompileError() << std::endl;
    }
    if (!parser_->GetErrorText().empty()) {
      os << "link error: \n" << parser_->GetErrorText() << std::endl;
    }
    return false;
  }

  TechniqueParser* tparser = parser_->GetTechniques();
  for (auto iter = tparser->GetTechniques().begin();
       iter != tparser->GetTechniques().end(); ++iter) {
    const TechniqueParser::Technique& tech = iter->second;
    AfxResult result;
    result.technique = &tech;
    GenHLSL(tech, &result);
    GenCppCode(tech, &result);
    resvec->push_back(result);
  }

  return true;
}

void AfxWrapper::GenHLSL(const TechniqueParser::Technique& tech, AfxResult* result) {
  int cnt = 0;
  result->hlsl.resize(kRenderPipelineStageNum);
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter, ++cnt) {
    if (!iter->entry) continue;

    azer::RenderPipelineStage stage = (azer::RenderPipelineStage)cnt;
    std::stringstream ss;
    HLSLCodeGeneratorFactory gen_factory;
    AfxCodegen codegen(&gen_factory);
    result->hlsl[cnt] = codegen.GenCode(stage, *iter, true);
  }
}

void AfxWrapper::GenCppCode(const TechniqueParser::Technique& tech,
                            AfxResult* result) {
  CppCodeGen codegen;
  codegen.GenCode(tech);
  result->hpp = codegen.GetHeadCode();
  result->cpp = codegen.GetCppCode();
}

}  // namespace afx
}  // namespace azer
