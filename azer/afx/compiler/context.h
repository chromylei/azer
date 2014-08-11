#pragma once

#include "azer/base/tree_node.h"
#include "azer/afx/compiler/debug.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/syntax_validator.h"

namespace azer {
namespace afx {

class ParseContext: public TreeNode<ParseContext> {
 public:
  struct Options {
    bool dump_tokenizer;
    bool dump_parser;
    bool syntax_valid;
    Options()
        : dump_tokenizer(false)
        , dump_parser(false)
        , syntax_valid(true) {
    }
  };
    
  ParseContext(const std::string& filepath, const std::string& package,
               const std::string& source, ASTNodeFactory* factory,
               Options opt = Options());
  ~ParseContext();
  
  ASTNode* Create(ASTNode::ASTNodeType type, const SourceLoc& loc);
  ASTNode* root() const { return root_;}

  void ReportError(const std::string& error, const char* cur);
  void ReportError(const SourceLoc& loc, const std::string& error);
  void ReportError(const SourceLoc& loc, const std::string& error, CompileErrno err);
  int lineno() const { return lineno_;}
  void lineno_plus() { ++lineno_;}
  const std::string& package() const { return package_;}
  const std::string& path() const { return filepath_;}

  bool success() const { return errno_ == kNoError;}
  CompileErrno GetErrno() { return errno_;}
  const std::string& source() const { return source_;}

  void AddAttributes(ASTNode* node);
  void AddVariableDecl(ASTNode* node);
  void AddStructDecl(ASTNode* node);
  bool AddFunction(ASTNode* node);
  bool AddTechnique(ASTNode* node);
  const ASTNodeVec& GetAttributes() const { return attributes_;}
  const ASTNodeDict& GetVariables() const { return variables_;}
  const ASTNodeDict& GetStructDecl() const { return struct_decl_;}
  const ASTNodeDict& GetFunctions() const { return functions_;}
  const ASTNodeDict& GetTechniques() const { return techniques_;}

  /**
   * 函数的查找是一个比较复杂的过程
   * 除了进行精确匹配之外，还可能对类型提升从而匹配
   * 例如函数 a(float,float)， 但我们使用 float(int,int) 时，没有精确匹配的函数
   * 但实际上却可以使用 a(float, float)， 当前不支持这种匹配方式
   */
  ASTNode* GetVariable(const std::string& name);
  ASTNode* GetFunction(const std::string& name);
  ASTNode* GetFunctionByName(const std::string& name);
  ASTNode* GetStructDecl(const std::string& name);

  /**
   * 查找符号对应的声明
   * 不提供 variable 的相关函数，不能跨包使用它们
   * Note: 函数的查找使用“函数签名”而非函数名
   */
  ASTNode* LookupFunction(const std::string& fullname);
  ASTNode* LookupFunction(const std::string& symbol, const std::string& name);
  ASTNode* LookupFuncHierarchy(const std::string& symbol, const std::string& name);
  ASTNode* LookupType(const std::string& fullname);
  ASTNode* LookupType(const std::string& symbol, const std::string& name);
  ASTNode* LookupTypeHierarchy(const std::string& symbol, const std::string& name);
  ASTNode* LookupBuiltinFunc(const std::string& funcname);

  /**
   * 此函数帮助完成编译阶段的语法检查及识别，具体任务包括
   * 1. 识别 identifier 是变量，函数，还是结构体
   * 2. 如果无法识别可以及时通报并报告错误
   */
  enum SymbolType {
    kUnknownSymbol = -1,
    kVariableSymbol = 1,
    kFunctionSymbol,
    kFunctionBuiltin,
    kStructureDecl,
    kExternalVariableSymbol,
    kExternFuncSymbol,
    kExternStructDecl,
  };
  
  SymbolType LookupSymbolType(const std::string& symbol);
  SymbolType LookupSymbolTypeFromDep(const std::string& package,
                                     const std::string& symbol);
  void RegisteSymbolType(const std::string& symbol, SymbolType type);
  std::ostream& tokenizer_stream();
  std::ostream& parser_stream();

  /**
   * internal funcition
   */
  void AppendExternal(const std::string& str);
  const std::string& GetExternPrefix() const { return extern_prefix_;}
  void ClearExternSymbol() { extern_prefix_.clear();}
  const std::vector<std::string>& GetErrorTextVec() const { return error_text_; }
  const Options& options() const { return options_;}
 private:
  SymbolType LookupSymbolTypeLocal(const std::string& symbol);

  std::string filepath_;
  std::string package_;
  int lineno_;
  bool success_;
  std::string source_;
  std::map<std::string, SymbolType> symbol_type_;
  ASTNodeFactory* factory_;
  ScopedNode* root_;

  // declaration
  std::vector<ASTNode*> attributes_;
  ASTNodeDict variables_;
  ASTNodeDict struct_decl_;
  ASTNodeDict functions_;
  ASTNodeDict techniques_;
  std::vector<std::string> error_text_;
  CompileErrno errno_;
  
  ASTNodeVec statement_;
  ASTNodeVec allocated_;
  std::stringstream null_stream_;
  std::string extern_prefix_;
  Options options_;

  friend class Parser;
  DISALLOW_COPY_AND_ASSIGN(ParseContext);
};

std::string SymbolTypeName(ParseContext::SymbolType type);
}  // namespace afx
}  // namespace azer
