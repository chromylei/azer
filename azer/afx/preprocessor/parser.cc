#include "azer/afx/preprocessor/parser.h"

#include <iostream>
#include "base/logging.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "preprocessor_tab.h"


extern int preprocessor_yyparse(void*);

namespace azer {
namespace afx {
bool PreprocessorParser::Parse(const std::string& code) {
  tokenizer_.reset(new PreprocessorTokenizer(code, this));
  preprocessor_yyparse(this);
  return !failed_;
}

int PreprocessorParser::tokenizer(YYSTYPE* yystyle) {
  DCHECK(tokenizer_.get() != NULL);
  yystyle->lex.string = NULL;
  return tokenizer_->GetNextToken(yystyle);
}

void PreprocessorParser::ReportError(const std::string& str, int lineno) {
  std::stringstream ss;
  ss << "Preprocessor " << lineno << ": " << str;
  error_text_.push_back(ss.str());
  failed_ = true;
}

void PreprocessorParser::ReportError(const std::string& str) {
  DCHECK(tokenizer_.get() != NULL);
  std::stringstream ss;
  ss << "Preprocessor " << tokenizer_->lineno() << ": " << str;
  error_text_.push_back(ss.str());
  failed_ = true;
}

void PreprocessorParser::AddDefined(const std::string& key) {
  defined_[key] = DefineInfo();
  if (parent_) parent_->AddDefined(key);
}

void PreprocessorParser::RemoveDefined(const std::string& key) {
  auto iter = defined_.find(key);
  if (iter != defined_.end()) defined_.erase(iter);
  if (parent_) parent_->RemoveDefined(key);
}

bool PreprocessorParser::IsDefined(const std::string& key) const {
  auto iter = defined_.find(key);
  return iter != defined_.end();
}

void PreprocessorParser::SetDefinedParams(const std::string& key,
                                          std::vector<std::string>* vec) {
  defined_[key].name = key;
  defined_[key].params.swap(*vec);
  if (parent_) parent_->SetDefinedParams(key, vec);
}

void PreprocessorParser::SetDefinedValue(const std::string& key,
                                         const std::string& value) {
  auto iter = defined_.find(key);
  DCHECK(iter != defined_.end());
  iter->second.name = key;
  iter->second.value = value;
  if (parent_) parent_->SetDefinedValue(key, value);
}

bool PreprocessorParser::GetDefindValue(const std::string& key,
                                        std::string* str) const {
  auto iter = defined_.find(key);
  if (iter == defined_.end()) {
    return false;
  } else {
    *str = iter->second.value;
    return true;
  }
}

std::string PreprocessorParser::GetDefinedValue(const std::string& key) const {
  auto iter = defined_.find(key);
  if (iter == defined_.end()) { return "";}
  else return iter->second.value;
}

std::ostream& PreprocessorParser::parser_stream() {
  if (trace_parse_) {return std::cout;}
  else { null_stream_.clear(); return null_stream_;}
}

void PreprocessorParser::AddIncludes(const std::string& str) {
  for (auto iter = includes_.begin(); iter != includes_.end(); ++iter) {
    if (*iter == str) return;
  }
  std::string output;
  ::base::TrimString(str, "\"", &output);
  includes_.push_back(output);

  if (delegate_) {
    delegate_->OnAddInclude(this, output);
  }
}

void PreprocessorParser::SetPackageName(const std::string& package) {
  ::base::TrimString(package, "; \t\n", &package_);
}
}  // namespace afx
}  // namespace azer
