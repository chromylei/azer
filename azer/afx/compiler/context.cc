#include "azer/afx/compiler/parser.h"

#include <iostream>

#include "azer/afx/compiler/builtin.h"
#include "azer/afx/compiler/errno.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/lazy_instance.h"
#include "lex.h"

extern int yyparse(void*);

namespace azer {
namespace afx {
// class ParseContext

ParseContext::ParseContext(const std::string& path, const std::string& package,
                           const std::string& source, ASTNodeFactory* factory,
                           Options opt)
    : filepath_(path)
    , package_(package)
    , lineno_(0)
    , source_(source)
    , factory_(factory)
    , root_(NULL)
    , errno_(kNoError)
    , options_(opt) {
  if (package != "__builtin__") {
    BuiltinContext::get();
  }
}

ParseContext::~ParseContext() {
  for (auto iter = allocated_.begin(); iter != allocated_.end(); ++iter) {
    delete *iter;
  }
  allocated_.clear();
}

std::ostream& ParseContext::tokenizer_stream() {
  if (options_.dump_tokenizer) {
    return std::cout;
  } else {
    return null_stream_;
  }
}

std::ostream& ParseContext::parser_stream() {
  if (options_.dump_parser) {
    return std::cout;
  } else {
    return null_stream_;
  }
}

void ParseContext::ReportError(const SourceLoc& loc, const std::string& error,
                               CompileErrno err) {
  errno_ = err;
  std::stringstream ss;
  ss << filepath_ << " line " << loc.lineno << ": " << error;
  error_text_.push_back(ss.str());
}

void ParseContext::ReportError(const std::string& error, const char* cur) {
  std::stringstream ss;
  ss << filepath_ << " line " << lineno() << ": " << error
     << " near \"" << cur << "\"";
  error_text_.push_back(ss.str());
  errno_ = kSyntaxError;
}

void ParseContext::ReportError(const SourceLoc& loc, const std::string& error) {
  std::stringstream ss;
  ss << filepath_ << " line " << loc.lineno << ": " << error;
  error_text_.push_back(ss.str());
  errno_ = kUnknownErr;
}

ASTNode* ParseContext::Create(ASTNode::ASTNodeType type, const SourceLoc& loc) {
  DCHECK(factory_ != NULL);
  ASTNode* node = factory_->Create(type, source(), loc);
  allocated_.push_back(node);
  node->context_ = this;
  return node;
}

void ParseContext::AddVariableDecl(ASTNode* node) {
  DCHECK(root_ != NULL);
  DCHECK(node->IsDeclarationNode());
  ASTNode* cur = node->first_child();
  while (cur) {
    if (cur->IsSymbolNode()) {
      SymbolNode* symbol = cur->ToSymbolNode();
      variables_.insert(std::make_pair(symbol->symbolname(), cur));
    }
    cur = cur->next_sibling();
  }
  root_->AddChildren(node);
}

void ParseContext::AddAttributes(ASTNode* node) {
  DCHECK(root_ != NULL);
  attributes_.push_back(node);
  root_->AddChildren(node);
}

void ParseContext::AddStructDecl(ASTNode* vnode) {
  DCHECK(root_ != NULL);
  DCHECK(vnode->IsStructDeclNode());
  StructDeclNode* node = vnode->ToStructDeclNode();
  struct_decl_.insert(std::make_pair(node->struct_name(), node));
  root_->AddChildren(node);
}

ASTNode* ParseContext::GetFunctionByName(const std::string& name) {
  std::string funcname = name;
  funcname.append("(");
  for (auto iter = functions_.begin(); iter != functions_.end(); ++iter) {
    if (StartsWithASCII(iter->first, funcname, true)) {
      return iter->second;
    }
  }
  return NULL;
}

bool ParseContext::AddFunction(ASTNode* vnode) {
  DCHECK(root_ != NULL);
  DCHECK(vnode->IsFuncDefNode());
  FuncDefNode* node = vnode->ToFuncDefNode();
  DCHECK(node->GetProtoNode() != NULL);
  std::string funcsync = std::move(node->GetProtoNode()->funcsync());
  auto iter = functions_.find(funcsync);
  if (iter == functions_.end()) {
    functions_.insert(std::make_pair(funcsync, node));
    root_->AddChildren(node);
    return true;
  } else {
    std::stringstream ss;
    ss << "function \"" << funcsync << "\" has been defined.";
    ReportError(vnode->loc(), ss.str(), kFuncDefined);
    return false;
  }
}

bool ParseContext::AddTechnique(ASTNode* node) {
  DCHECK(root_ != NULL);
  DCHECK_EQ(node->type(), ASTNode::kAttributesNode);
  AttributesNode* attr = node->ToAttributesNode();
  std::string name = attr->GetAttrValue("name");
  if (name.empty()) {
    ReportError(node->loc(), "Attributes lack field \"name\"", kInvalidAttr);
    return false;
  }

  auto iter = techniques_.find(name);
  if (iter == techniques_.end()) {
    techniques_.insert(std::make_pair(name, node));
    root_->AddChildren(node);
    return true;
  } else {
    std::stringstream ss;
    ss << "function \"" << name << "\" has been defined.\n";
    ReportError(node->loc(), ss.str(), kInvalidAttr);
    return false;
  }
}

ParseContext::SymbolType ParseContext::LookupSymbolTypeLocal(
    const std::string& symbol) {
  auto iter = symbol_type_.find(symbol);
  if (iter != symbol_type_.end()) { return iter->second; }
  return kUnknownSymbol;
}

ParseContext::SymbolType ParseContext::LookupSymbolType(const std::string& symbol) {
  if (extern_prefix_.empty()) {
    return LookupSymbolTypeLocal(symbol);
  } else {
    return LookupSymbolTypeFromDep(extern_prefix_, symbol);
  }
}

void ParseContext::AppendExternal(const std::string& str) {
  if (!extern_prefix_.empty()) {
    extern_prefix_.append("::");
  }
  extern_prefix_.append(str.substr(0, str.length() - 2));
}


ParseContext::SymbolType ParseContext::LookupSymbolTypeFromDep(
    const std::string& package, const std::string& symbol) {
  DCHECK(!EndsWith(package, "::", true));
  SymbolType stype = kUnknownSymbol;
  if (package_ == package) {
    stype = LookupSymbolType(symbol);
  }

  ParseContext* cur = first_child();
  while (cur && stype == kUnknownSymbol) {
    stype = cur->LookupSymbolTypeFromDep(package, symbol);
    cur = cur->next_sibling();
  }
  
  switch (stype) {
    case kVariableSymbol: return kExternalVariableSymbol;
    case kFunctionSymbol: return kExternFuncSymbol;
    case kStructureDecl: return kExternStructDecl;
    case kUnknownSymbol: return kUnknownSymbol;
    default: return stype;
  }
}

ASTNode* ParseContext::GetVariable(const std::string& name) {
  auto iter = variables_.find(name);
  if (iter == variables_.end()) return NULL;
  else return iter->second;
}

ASTNode* ParseContext::GetFunction(const std::string& name) {
  auto iter = functions_.find(name);
  if (iter == functions_.end()) return NULL;
  else return iter->second;
}

ASTNode* ParseContext::GetStructDecl(const std::string& name) {
  auto iter = struct_decl_.find(name);
  if (iter == struct_decl_.end()) return NULL;
  else return iter->second;
}

void ParseContext::RegisteSymbolType(const std::string& symbol, SymbolType type) {
  DCHECK(symbol_type_.find(symbol) == symbol_type_.end())
      << "symbol \"" << symbol << "\" has been declared.";
  symbol_type_.insert(std::make_pair(symbol, type));
}

ASTNode* ParseContext::LookupFunction(const std::string& fullname) {
  uint32 pos = fullname.find_last_of("::");
  if (pos != std::string::npos) {
    std::string package = fullname.substr(0, pos-1);
    std::string name = fullname.substr(pos + 1);
    return LookupFunction(package, name);
  } else {
    return LookupFunction("", fullname);
  }
}

ASTNode* ParseContext::LookupBuiltinFunc(const std::string& funcname) {
  return BuiltinContext::Lookup(funcname);
}

ASTNode* ParseContext::LookupFunction(const std::string& package,
                                      const std::string& name) {
  DCHECK(!EndsWith(package, "::", true));
  ASTNode* node = NULL;
  if (package_ == package) {
    node = GetFunction(name);
  }

  if (node == NULL) {
    return LookupFuncHierarchy(package, name);
  } else {
    return node;
  }
}

ASTNode* ParseContext::LookupFuncHierarchy(const std::string& symbol,
  const std::string& name) {
  ParseContext* cur = first_child();
  while (cur) {
    ASTNode* node = cur->LookupFunction(symbol, name);
    if (node) return node;
    cur = cur->next_sibling();
  }
  return NULL;
}

ASTNode* ParseContext::LookupType(const std::string& fullname) {
  uint32 pos = fullname.find_last_of("::");
  if (pos != std::string::npos) {
    std::string package = fullname.substr(0, pos-1);
    std::string name = fullname.substr(pos + 1);
    return LookupType(package, name);
  } else {
    return LookupType("", fullname);
  }
}

ASTNode* ParseContext::LookupType(const std::string& package,
                                  const std::string& name) {
  DCHECK(!EndsWith(package, "::", true));
  ASTNode* node = NULL;
  if (package_ == package) {
    node = GetStructDecl(name);
  }

  if (node == NULL) {
    return LookupTypeHierarchy(package, name);
  } else {
    return node;
  }
}

ASTNode* ParseContext::LookupTypeHierarchy(const std::string& symbol,
  const std::string& name) {
  ParseContext* cur = first_child();
  while (cur) {
    ASTNode* node = cur->LookupType(symbol, name);
    if (node) return node;
    cur = cur->next_sibling();
  }
  return NULL;
}

std::string SymbolTypeName(ParseContext::SymbolType type) {
  switch (type) {
    case ParseContext::kVariableSymbol: return "variable";
    case ParseContext::kFunctionSymbol: return "function";
    case ParseContext::kStructureDecl: return "structure";
    case ParseContext::kExternFuncSymbol: return "extern function";
    case ParseContext::kExternStructDecl: return "extern structure";
    case ParseContext::kUnknownSymbol:
    default:
      NOTREACHED(); return "";
  }
}
}  // namespace afx
}  // namespace azer
