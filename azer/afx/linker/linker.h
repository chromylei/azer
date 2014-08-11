#pragma once

#include <map>
#include <string>

#include "base/basictypes.h"
#include "azer/afx/linker/program.h"
#include "azer/afx/util/file_loader.h"

namespace azer {
namespace afx {

/**
 * AfxLinker
 * 完成两个功能
 * 1. 将依赖的文件在加载起来并检查循环依赖关系
 * 2. 查找并连接外部符号
 **/

class AfxLinker : public PreprocessorParser::Delegate {
 public:
  struct Options {
    bool dump_includes_file;
    bool dump_defined;
    bool dump_defined_file;
    bool parse_astree;
    Options()
        : dump_includes_file(false)
        , dump_defined(false)
        , dump_defined_file(false)
        , parse_astree(true) {
    }
  };

  AfxLinker() : failed_(false) {}
  explicit AfxLinker(FileLoader* loader, const Options& opt)
      : loader_(loader)
      , options_(opt)
      , failed_(false) {
  }
  ~AfxLinker();

  Program* root() { return root_.get();}
  bool Load(const std::string& content, const std::string& path);
  bool success() const { return !failed_;}
  std::string GetCompileError() const;
  const std::string& GetErrorText() const { return error_text_;}
 protected:
  /**
   * 此函数使用虚函数是为了简化单元测试流程
   */
  bool LoadFile(const std::string& file, std::string* content,
                ::base::FilePath* finaly);
 private:
  virtual void OnAddInclude(PreprocessorParser* parser,
                            const std::string& path) OVERRIDE;
  void ReportError(const std::string& error);

  bool ParseASTreeRecursive(Program* program, Program* parent);

  std::unique_ptr<Program> root_;
  std::vector<Program*> program_vec_;

  /**
   * 此结构体用于帮助根据 package name 查找对应的程序
   */
  std::multimap<std::string, Program> package_map_;
  const Options options_;
  FileLoader* loader_;
  bool failed_;
  std::string error_text_;
  DISALLOW_COPY_AND_ASSIGN(AfxLinker);
};
}  // namespace afx
}  // namespace azer
