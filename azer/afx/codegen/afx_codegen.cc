#include "azer/afx/codegen/afx_codegen.h"

#include <sstream>

#include "azer/afx/codegen/code_generator.h"
#include "azer/afx/codegen/hlsl_util.h"
#include "azer/afx/codegen/tex_util.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/debug.h"
#include "base/logging.h"

namespace azer {
namespace afx {

namespace {
bool IsConstBufferMember(ASTNode* node) {
  DCHECK(node->IsDeclarationNode());
  DeclarationNode* decl = node->ToDeclarationNode();
  TypePtr typeptr = decl->GetType();
  if (typeptr->IsStructure()) {
    StructDeclNode* struct_decl = decl->GetTypedNode()->GetStructDecl();
    DCHECK(NULL != struct_decl);
    return !HasOnlyTextureField(struct_decl);
  } else {
    return true;
  }
}
}  // namespace
std::string AfxCodegen::GenDeps(const TechniqueParser::StageInfo& shader,
                                bool comments) {
  std::stringstream ss;
  for (auto iter = shader.depend.rbegin(); iter != shader.depend.rend(); ++iter) {
    ASTNode* node = *iter;
    ss << std::move(GenDepend(node, comments));
  }

  return ss.str();
}

std::string AfxCodegen::GenUniformDeps(const TechniqueParser::StageInfo& shader,
                                       bool comments) {
  std::stringstream ss;
  for (auto iter = shader.uni_depend.rbegin();
       iter != shader.uni_depend.rend(); ++iter) {
    ASTNode* node = *iter;
    ss << std::move(GenUniDepend(node, comments));
  }

  return ss.str();
}

std::string AfxCodegen::GenCode(RenderPipelineStage stage,
                                const TechniqueParser::StageInfo& shader,
                                bool comments) {
  DCHECK(stage_ == kStageNotSpec);
  DCHECK(stage != kStageNotSpec);
  stage_ = stage;
  std::vector<ASTNode*> uniforms;
  std::stringstream ss;

  LOG(ERROR) << shader.entry->GetContext()->path();
  if (comments) {
    ss << "/**\n"
       << " * code generated by afxc, donnot modify by hand\n"
       << " * afx code if from file: \n"
       << " *   " << shader.entry->GetContext()->path() << "\n"
       << " */\n\n";
  }

  
  ss << "// using row_major\n"
     << "#pragma pack_matrix(row_major)\n\n";
  ss << std::move(GenDeps(shader, comments));
  ss << std::move(GenUniformDeps(shader, comments));
  ss << std::move(GenUniform(shader.uniforms, comments));
  ss << std::move(GenTextures(shader.uni_textures, comments));
  ss << std::move(GenEntry(shader.entry, comments));
  
  return ss.str();
}

std::string AfxCodegen::GenEntry(ASTNode* node, bool comments) {
  SnippetCodeGenerator generator(factory_);
  DCHECK(node);
  generator.GenCode(node);
  return std::move(generator.GetCode());
}

std::string AfxCodegen::GenTextures(const std::vector<ASTNode*>& textures,
                                    bool comments) {
  std::stringstream ss;
  for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
    ASTNode* node = *iter;
    ss << HLSLTextureTypeName(node) << " "
       << HLSLDeclaredTextureFullName(node) << ";\n";
    if (stage_ == kPixelStage) {
      ss << "SamplerState " << HLSLUniformTextureSamplerDeclFullName(node) << ";\n";
    }
  }
  return ss.str();
}

std::string AfxCodegen::GenUniform(const std::vector<ASTNode*> &uniforms,
                                   bool comments) {
  std::stringstream ss;
  // sort uniforms
  if (uniforms.size() == 0u) return "";
  ss << "cbuffer c_buffer {";
  for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
    DCHECK((*iter)->IsDeclarationNode());
    DeclarationNode* decl = (*iter)->ToDeclarationNode();
    if (!IsConstBufferMember(decl)) {
      continue;
    }
    DCHECK (!decl->GetType()->IsTexture());
    SnippetCodeGenerator generator(factory_);
    generator.GenCode(decl);
    ss << std::move(generator.GetCode());
  }
  ss << "};";
  return ss.str();
}

std::string AfxCodegen::GenDepend(ASTNode* node, bool comments) {
  std::stringstream ss;
  DCHECK(factory_ != NULL);
  if (comments) {
    ss << "// genereated code from for \"" << ASTNodeName(node->type()) << "\"\n"
       << "// which defined in file:\n"
       << "//   " << node->GetContext()->path() << "\n";
  }
  SnippetCodeGenerator generator(factory_);
  generator.GenCode(node);
  ss << std::move(generator.GetCode());
  if (node->IsStructDeclNode()) {
    type_depends_.insert(StructFullName(node));
  }
  return ss.str();
}

std::string AfxCodegen::GenUniDepend(ASTNode* node, bool comments) {
  if (HasOnlyTextureField(node)) {
    return "";
  }

  if (node->IsStructDeclNode()
      && type_depends_.find(StructFullName(node)) != type_depends_.end()) {
       return "";
  }

  std::stringstream ss;
  DCHECK(factory_ != NULL);
  if (comments) {
    ss << "// genereated code from for \"" << ASTNodeName(node->type()) << "\"\n"
       << "// which defined in file:\n"
       << "//   " << node->GetContext()->path() << "\n";
  }
  SnippetCodeGenerator generator(factory_);
  generator.GenCode(node);
  ss << std::move(generator.GetCode());
  return ss.str();
}

}  // namespace afx
}  // namespace azer
