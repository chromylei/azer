#pragma once

#include <memory>
#include <vector>
#include <string>

#include "base/basictypes.h"
#include "azer/afx/linker/afx_parser.h"

class AfxWrapper {
 public:
  explicit AfxWrapper(const std::string& includes);

  struct AfxResult {
    std::string cpp;
    std::string hpp;
    std::vector<std::string> hlsl;
    std::vector<std::string> glsl;
    azer::afx::TechniqueParser::Technique* technique;
  };
  bool Parse(const ::base::FilePath& path, std::ostream& os,
             std::vector<AfxResult>* result); 
 private:
  void GenCppCode(const azer::afx::TechniqueParser::Technique& tech,
                  AfxResult* result);
  void GenHLSL(const azer::afx::TechniqueParser::Technique& tech, AfxResult* result);
  std::vector<std::string> includes_;
  DISALLOW_COPY_AND_ASSIGN(AfxWrapper);
};
