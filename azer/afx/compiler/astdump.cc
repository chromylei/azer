#include "azer/afx/compiler/astdump.h"

#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/type.h"

namespace azer {
namespace afx {
std::string DumpNodeInfo(ASTNode* node);
// class ASTreeDumper
void ASTreeDumper::Dump(ASTNode* node, std::string* str) {
  str->clear();
  dump_str_.clear();
  node->traverse(this);
  str->swap(dump_str_);
}


bool ASTreeDumper::OnVisitBegin(ASTNode* node) {
  std::string ident_str(depth(), ' ');
  std::stringstream ss;
  ss << ident_str << DumpNodeInfo(node) << "\n";
  dump_str_.append(ss.str());
  depth_++;
  return true;
}

void ASTreeDumper::OnVisitEnd(ASTNode* node) {
  depth_--;
}

std::string DumpBinaryOpNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kBinaryOpNode);
  std::stringstream ss;
  BinaryOpNode* binary = node->ToBinaryOpNode();
  ss << ASTNodeName(node->type()) << "[" << operator_str(binary->GetOperator())
     << "]";
  return ss.str();
}

std::string DumpConstNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kConstNode);
  std::stringstream ss;
  ConstNode* cnode = node->ToConstNode();
  ss << ASTNodeName(node->type()) << "{";
  BasicType basic_type = cnode->GetResultType()->type();
  switch (basic_type) {
    case kBoolean: ss << cnode->value()->GetBoolean(); break;
    case kChar:   ss << cnode->value()->GetChar(); break;
    case kShort:  ss << cnode->value()->GetShort(); break;
    case kUint32: ss << cnode->value()->GetUint32(); break;
    case kInt32: ss << cnode->value()->GetInt32(); break;
    case kFloat: ss << cnode->value()->GetFloat(); break;
    default: NOTREACHED() << "unknown type: " << basic_type;
  }
  ss << "}";
  return ss.str();
}

std::string DumpFieldNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kFieldNode);
  std::stringstream ss;
  FieldNode* field = node->ToFieldNode();
  ss << ASTNodeName(node->type()) << "[" << field->fieldname() << "]";
  return ss.str();
}

std::string DumpFuncCallNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kFuncCallNode);
  std::stringstream ss;
  FuncCallNode* func = node->ToFuncCallNode();
  ss << ASTNodeName(node->type()) << "{"  << func->funcname() << "}";
  return ss.str();
}

std::string DumpFuncCallTypeInitNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kFuncCallTypeInitNode);
  std::stringstream ss;
  FuncCallTypeInitNode* func = node->ToFuncCallTypeInitNode();
  ss << ASTNodeName(node->type()) << "{"
     << type_name(func->GetType()->type()) << "}";
  return ss.str();
}

std::string DumpFuncDefNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kFuncDefNode);
  std::stringstream ss;
  FuncDefNode* func = node->ToFuncDefNode();
  ss << ASTNodeName(node->type()) << "{"  << func->funcname() << "}";
  return ss.str();
}

std::string DumpFuncProtoNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kFuncProtoNode);
  std::stringstream ss;
  FuncProtoNode* func = node->ToFuncProtoNode();
  ss << ASTNodeName(node->type()) << "{"  << func->funcname() << "}";
  return ss.str();
}

std::string DumpRefSymbolNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kRefSymbolNode);
  std::stringstream ss;
  RefSymbolNode* ref = node->ToRefSymbolNode();
  ss << ASTNodeName(node->type()) << "[" << ref->symbolname() << "]";
  return ss.str();
}

std::string DumpParamNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kParamNode);
  std::stringstream ss;
  ParamNode* param = node->ToParamNode();
  ss << ASTNodeName(node->type()) << "[" << param->paramname() << "]";
  return ss.str();
}

std::string DumpSymbolNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kSymbolNode);
  std::stringstream ss;
  SymbolNode* ref = node->ToSymbolNode();
  ss << ASTNodeName(node->type()) << "{";

  ss << ref->symbolname();
  if (ref->GetType()->IsArray()) { ss << "[]";}
  ss << "}";
  return ss.str();
}

std::string DumpActParamNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kActParamNode);
  std::stringstream ss;
  ActParamNode* ref = node->ToActParamNode();
  ss << ASTNodeName(node->type()) << "{";

  ss << ref->symbolname();
  if (ref->GetType()->IsArray()) { ss << "[]";}
  ss << "}";
  return ss.str();
}

std::string DumpUnaryOpNode(ASTNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kUnaryOpNode);
  std::stringstream ss;
  UnaryOpNode* unary = node->ToUnaryOpNode();
  ss << ASTNodeName(node->type()) << "[" << operator_str(unary->GetOperator())
     << "]";
  return ss.str();
}

std::string DumpNodeInfo(ASTNode* node) {
  switch (node->type()) {
      /*
    case ASTNode::kAttributesNode:
      return new AttributesNode(source, loc);
      */
    case ASTNode::kBinaryOpNode:
      return DumpBinaryOpNode(node);
      /*
    case ASTNode::kCaseNode:
      return new CaseNode(source, loc);
    case ASTNode::kConditionalOpNode:
      return new ConditionalOpNode(source, loc);
      */
    case ASTNode::kConstNode:
      return DumpConstNode(node);
      /*
    case ASTNode::kDeclarationNode:
      return new DeclarationNode(source, loc);
      */
    case ASTNode::kFieldNode:
      return DumpFieldNode(node);
      /*
    case ASTNode::kForLoopNode:
      return new ForLoopNode(source, loc);
      */
    case ASTNode::kFuncCallNode:
      return DumpFuncCallNode(node);
    case ASTNode::kFuncCallTypeInitNode:
      return DumpFuncCallTypeInitNode(node);
    case ASTNode::kFuncDefNode:
      return DumpFuncDefNode(node);
    case ASTNode::kFuncProtoNode:
      return DumpFuncProtoNode(node);
      /*
    case ASTNode::kIfElseNode:
      return new IfElseNode(source, loc);
    case ASTNode::kInitializerNode:
      return new InitializerNode(source, loc);
    case ASTNode::kJumpNode:
      return new JumpNode(source, loc);
    case ASTNode::kNullNode:
      return new NullNode(source, loc);
      */
    case ASTNode::kParamNode:
      return DumpParamNode(node);
    case ASTNode::kRefSymbolNode:
      return DumpRefSymbolNode(node);
      /*
    case ASTNode::kReturnNode:
      return new ReturnNode(source, loc);
    case ASTNode::kScopedNode:
      return new ScopedNodeHLSLCodeGen(node);
    case ASTNode::kStructDeclNode:
      return new StructDeclNodeHLSLCodeGen(node);
    case ASTNode::kSwitchNode:
    return new SwitchNode(source, loc);
      */
    case ASTNode::kSymbolNode:
      return DumpSymbolNode(node);
    case ASTNode::kActParamNode:
      return DumpActParamNode(node);
    case ASTNode::kUnaryOpNode:
      return DumpUnaryOpNode(node);
      /*
    case ASTNode::kWhileLoopNode: 
      return new WhileLoopNode(source, loc);
      */
    default:
      return ASTNodeName(node->type());
  }
}
}  // namespace afx
}  // namespace azer
