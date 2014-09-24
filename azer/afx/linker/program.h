#pragma once

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/base/tree_node.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/preprocessor/parser.h"

namespace azer {
namespace afx {

class Program : public TreeNode<Program> {
 public:
  Program(Program* parent, const ::base::FilePath& path, const std::string& code)
      : path_(path)
      , code_(code) {
    if (parent) {parent->AddChild(this);}
  }

  ASTNode* LookupSymbol(const std::string& symbol);
  const PreprocessorParser::IncludedVec&  includes() const;

  bool ParseASTree(Program* parent);
  bool Preprocess(PreprocessorParser::Delegate* delegate);
  std::string GenCode();
  const ::base::FilePath::StringType& path() const { return path_.value();}
  ParseContext* GetContext() { return context_.get();}
 private:
  ::base::FilePath path_;
  std::string code_;
  ASTNodeFactory factory_;
  std::unique_ptr<PreprocessorParser> preprocessor_;
  std::unique_ptr<ParseContext> context_;
  
  friend class AfxLinker;
  DISALLOW_COPY_AND_ASSIGN(Program);
};
  
}  // namespace afx
}  // namespace azer
