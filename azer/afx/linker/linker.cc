#include "azer/afx/linker/linker.h"

#include "azer/afx/compiler/afxl.h"
#include "azer/base/string.h"
#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"

using ::base::FilePath;

namespace azer {
namespace afx {

AfxLinker::~AfxLinker() {
  for (auto iter = program_vec_.begin(); iter != program_vec_.end(); ++iter) {
    delete *iter;
  }
  program_vec_.clear();
}

void AfxLinker::OnAddInclude(PreprocessorParser* parser, const FilePath& path) {
  std::string code;
  ::base::FilePath finalpath;
  if (!LoadFile(path, &code, &finalpath)) {
    std::stringstream ss;
    ss << "Failed to load file: \"" << path.value() << "\"";
    ReportError(ss.str());
    return;
  }

  Program* parent = (Program*)parser->extra();
  Program* newprog = new Program(parent, path, code);
  if (!newprog->Preprocess(this)) {
    std::stringstream ss;
    ss << "Failed to parse file: \"" << path.value() << "\"";
    ReportError(ss.str());
  }
}

void AfxLinker::ReportError(const std::string& str) {
  error_text_ = str;
  failed_ = true;
}

namespace {
class ErrorCollector : public TreeNode<Program>::Traverser {
 public:
  virtual bool OnVisitBegin(Program* p) { return true;}
  virtual void OnVisitEnd(Program* p) {
    for (auto iter = p->GetContext()->GetErrorTextVec().begin();
         iter != p->GetContext()->GetErrorTextVec().end(); ++iter) {
      error_.push_back(*iter);
    }
  }

  const std::vector<std::string>& GetError() const { return error_;}
 private:
  std::vector<std::string> error_;
};
}  // namespace

std::string AfxLinker::GetCompileError() const {
  std::stringstream ss;
  ErrorCollector collector;
  if (root_.get()) root_->traverse(&collector);
  for (auto iter = collector.GetError().begin();
       iter != collector.GetError().end(); ++iter) {
    ss << *iter << std::endl;
  }
  return ss.str();
}

bool AfxLinker::Load(const std::string& content, const ::base::FilePath& path) {
  root_.reset(new Program(NULL, path, content));
  if (!root_->Preprocess(this)) {
    failed_ = true;
    return false;
  }

  /**
   * parse root Abstract Syntax Tree and lookup external symbol
   */
  if (failed_) return false;
  return ParseASTreeRecursive(root_.get(), NULL);
}

bool AfxLinker::ParseASTreeRecursive(Program* program, Program* parent) {
  Program* cur = program->first_child();
  while (cur) {
    if (cur->GetContext()->package().empty()) {
      std::stringstream ss;
      ss << "compile failed: included afx file need package, file: \""
         << cur->path() << "\"";
      ReportError(ss.str());
      return false;
    }

    if (!ParseASTreeRecursive(cur, program)) {
      return false;
    }
    cur = cur->next_sibling();
  }

  if (!program->ParseASTree(parent)) {
    ReportError("");  // compile failed
  }
  return !failed_;
}

bool AfxLinker::LoadFile(const ::base::FilePath& file, std::string* content,
                         ::base::FilePath* finalpath) {
  DCHECK(NULL != loader_);
  return loader_->Load(file, content, finalpath);
}

}  // namespace afx
}  // namespace azer
