/***
 * 预编译处理器
 * 预编译处理的结果与语法处理的结果不同，他的输出包括三个部分
 * 第一部分：include package
 * 第二部分：定义的常量
 * 第三部分: 有效的代码
 *
 * Note： 预编译常量将不再预编译阶段计算，而是在 parse ASTTree
 * 过程中，直接进行替换
 * 这些常量包括: __FILE__, __LINE__ 等
 *
 */
#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/loc.h"
#include "azer/afx/preprocessor/tokenizer.h"

namespace azer {
namespace afx {

class PreprocessorParser : public PreprocessorTokenizer::Delegate {
 public:
  class Delegate {
   public:
    virtual void OnAddInclude(PreprocessorParser* parser, const std::string& path) = 0;
  };

  explicit PreprocessorParser(Delegate* delegate, bool trace)
      : parent_(NULL)
      , delegate_(delegate)
      , trace_parse_(trace)
      , failed_(false)
      , extra_(NULL) {
  }
  
  PreprocessorParser(PreprocessorParser* parent, Delegate* delegate,
                     bool trace = false)
      : parent_(parent)
      , delegate_(delegate)
      , trace_parse_(trace)
      , failed_(false)
      , extra_(NULL) {
  }

  bool Parse(const std::string& code);
  const std::string& GetCode() const { return code_; }

  int tokenizer(YYSTYPE* yystyle);  // called by preprocessor_yyparse
  virtual void ReportError(const std::string& str, int lineno);
  void ReportError(const std::string& str);

  struct DefineInfo {
    std::string name;
    std::vector<std::string> params;
    std::string value;
  };

  // extern interface
  typedef std::vector<std::string> IncludedVec;
  const IncludedVec& includes() const { return includes_;}
  const std::string& package() const { return package_;}

  void* extra() const { return extra_; }
  void set_extra(void* ex) { extra_ = ex;}

  // interface for parser
  typedef std::map<std::string, DefineInfo> DefineInfoDict;
  bool IsDefined(const std::string& key) const;
  void AddDefined(const std::string& key);
  void RemoveDefined(const std::string& key);
  void SetDefinedParams(const std::string& key, std::vector<std::string>* vec);
  void SetDefinedValue(const std::string& key, const std::string& value);
  const DefineInfoDict& defined() const { return defined_;}
  std::string GetDefinedValue(const std::string& key) const;
  bool GetDefindValue(const std::string& key, std::string* str) const;

  // add include and parse the include
  void AddIncludes(const std::string& str);
  // set packagename
  void SetPackageName(const std::string& package);
  void AppendCode(const std::string& code) {code_.append(code);}
  std::ostream& parser_stream();

  const std::vector<std::string>& GetErrorText() const { return error_text_;}
 private:
  PreprocessorParser* parent_;
  Delegate* delegate_;
  DefineInfoDict defined_;
  std::string code_;
  std::stringstream null_stream_;
  std::string package_;
  IncludedVec includes_;
  bool trace_parse_;
  bool failed_;
  void* extra_;
  std::unique_ptr<PreprocessorTokenizer> tokenizer_;
  std::vector<std::string> error_text_;
  DISALLOW_COPY_AND_ASSIGN(PreprocessorParser);
};
}  // namespace afx
}  // namespace azer
