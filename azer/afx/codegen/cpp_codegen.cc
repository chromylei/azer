#include "azer/afx/codegen/cpp_codegen.h"

#include <sstream>
#include <ctype.h>
#include <set>

#include "azer/render/render_system_enum.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/codegen/hlsl_util.h"
#include "azer/afx/codegen/tex_util.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"

namespace azer {
namespace afx {

namespace {
const char* StageName(RenderPipelineStage stage);
const char* StagePrefix(RenderPipelineStage stage);
std::string GpuTableDescVarName(RenderPipelineStage stage);
const char* GpuConstantTypeFromUniform(const TypePtr& typeptr);
std::string GetTexVarName(ASTNode* node, RenderPipelineStage stage);
std::string SelfDefStructureName(StructDeclNode* node);
bool IsSelfDefinedStruct(const TypePtr& typeptr);
std::string GetSemanticName(const std::string& name);
int GetSemanticIndex(FieldNode* field);

bool IsExternCppStruct(StructDeclNode* node);
std::string UniformTypeName(ASTNode* node);
std::string UniformTypeIndex(const TypePtr& type);
std::string GetStructTypeName(ASTNode* node);
}  // namespace

CppCodeGen::CppCodeGen() {
}

CppCodeGen::~CppCodeGen() {
}

std::string CppCodeGen::GenStageTexture(RenderPipelineStage stage,
                                        const ASTNodeVec& textures) {
  std::stringstream ss;
  int index = 0;
  for (auto iter = textures.begin(); iter != textures.end(); ++iter, ++index) {
    std::string symbolname = GetTexVarName(*iter, stage);
    std::string funcname = HLSLTextureUniformFuncName(*iter);;
    if (funcname.empty()) {
      funcname = std::string("set_") + symbolname;
    }

    if (IsTextureArray(*iter)) {
      ss << "  void " << funcname
         << "(int index, const azer::TexturePtr& texture) {\n"
         << "    DCHECK_LT(index, (int)arraysize(" << symbolname << "));\n"
         << "    " << symbolname << "[index] = texture;\n"
         << "  }\n";
    } else {
      ss << "  void " << funcname << "(const azer::TexturePtr& texture) {\n"
         << "    " << symbolname << " = texture;\n"
         << "  }\n";
    }
  }
  return ss.str();
}

std::string CppCodeGen::GenStageUniforms(RenderPipelineStage stage,
                                         const ASTNodeVec& uniforms) {
  std::stringstream ss;
  int index = 0;
  for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter, ++index) {
    DCHECK((*iter)->IsDeclarationNode());
    DeclarationNode* decl = (*iter)->ToDeclarationNode();
    DCHECK(decl->last_child()->IsSymbolNode());
    SymbolNode* symbol = decl->last_child()->ToSymbolNode();
    TypePtr typeptr = symbol->GetType();
    std::string funcname;
    if (decl->attributes() && decl->attributes()->HasAttr("uniform_func")) {
      funcname = decl->attributes()->GetAttrValue("uniform_func");
    } else {
      std::string symbolnode = symbol->symbolname();
      funcname = std::string("set_") + StagePrefix(stage) + "_" + symbolnode;
    }

    DCHECK(!decl->GetType()->IsTexture());
    ss << "  void " << funcname << "(const " << UniformTypeName(decl);
    if (typeptr->IsArray()) {
      ss << "* value, int num) {\n"
         << "    azer::GpuConstantsTable* tb = gpu_table_[(int)"
         << StageName(stage) << "].get();\n"
         << "    DCHECK(tb != NULL);\n"
         << "    tb->SetValue(" << index << ", value, "
         << "sizeof(" << UniformTypeName(decl) << ") * num);\n"
         << "  }\n";
    } else {
      ss << "& value) {\n"
         << "    azer::GpuConstantsTable* tb = gpu_table_[(int)"
         << StageName(stage) << "].get();\n"
         << "    DCHECK(tb != NULL);\n"
         << "    tb->SetValue(" << index << ", &value, "
         << "sizeof(" << UniformTypeName(decl) << "));\n"
         << "  }\n";
    }
  }
  return ss.str();
}

std::string CppCodeGen::GenUniformFuncs(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  int cnt = 0;
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter, ++cnt) {
    if (iter->entry == NULL) continue;
    RenderPipelineStage stage = (RenderPipelineStage)cnt;
    ss << std::move(GenStageUniforms(stage, iter->uniforms));
    ss << std::move(GenStageTexture(stage, iter->uni_textures));
  }

  return ss.str();
}


std::string CppCodeGen::GenStageGpuTableDesc(RenderPipelineStage stage,
                                             const ASTNodeVec& uniforms) {
  if (uniforms.size() == 0u) return "";
  std::stringstream ss;
  ss << "  azer::GpuConstantsTable::Desc " << GpuTableDescVarName(stage)
     << "[] = {\n";
  for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
    DCHECK((*iter)->IsDeclarationNode());
    DeclarationNode* decl = (*iter)->ToDeclarationNode();
    DCHECK(decl->last_child() && decl->last_child()->IsSymbolNode());
    SymbolNode* symbol = decl->last_child()->ToSymbolNode();
    TypePtr type = symbol->GetType();
    DCHECK(!type->IsTexture());
    if (!IsSelfDefinedStruct(type)) {
      ss << "    azer::GpuConstantsTable::Desc(\"" << symbol->symbolname() << "\", "
         << GpuConstantTypeFromUniform(type) << ",\n"
         << "         offsetof(" << StagePrefix(stage) << "_cbuffer, "
         << symbol->symbolname() << "), "
         << (type->IsArray() ? type->GetDim(0) : 1) << "),\n";
    } else {
      TypedNode* typed = symbol->GetTypedNode();
      DCHECK(typed != NULL);
      StructDeclNode* decl = typed->GetStructDecl();
      DCHECK(decl != NULL);
      ss << "    azer::GpuConstantsTable::Desc(\"" << symbol->symbolname()
         << "\", offsetof(" << StagePrefix(stage) << "_cbuffer, "
         << symbol->symbolname() << "),\n"
         << "         sizeof(" << SelfDefStructureName(decl) << "), "
         << (type->IsArray() ? type->GetDim(0) : 1) << "),\n";
    }
  };
  ss << "  };\n";
  return ss.str();
}

std::string CppCodeGen::GenStageGpuTable(RenderPipelineStage stage,
                                         const ASTNodeVec& uniforms) {
  if (uniforms.size() == 0u) return "";

  int index = 0;
  std::stringstream ss;
  ss << "  // generate GpuTable init for stage " << StageName(stage) << "\n";
  ss << std::move(GenStageGpuTableDesc(stage, uniforms));
  ss << "  gpu_table_[" << StageName(stage) << "]"
     << ".reset(render_system_->CreateGpuConstantsTable(\n"
     << "      arraysize(" << GpuTableDescVarName(stage) << "), "
     << GpuTableDescVarName(stage) << "));\n";
  return ss.str();
}

std::string CppCodeGen::GenTechnique(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  ss << "void " << GetClassName(tech) << "::InitTechnique() {\n"
     << "  technique_.reset(render_system_->CreateTechnique());\n"
     << "  vertex_desc_ptr_.reset(new azer::VertexDesc(kVertexDesc, "
     << "kVertexDescNum));\n"
     << "  const std::string& vs_shader_source = sources_[azer::kVertexStage];\n"
     << "  DCHECK(!vs_shader_source.empty());\n"
     << "  azer::GpuProgramPtr vs_gpup_ptr(render_system_->CreateVertexGpuProgram(\n"
     << "      vertex_desc_ptr_, vs_shader_source));\n"
     << "  DCHECK(vs_gpup_ptr.get() != NULL);\n"
     << "  technique_->AddGpuProgram(vs_gpup_ptr);\n";

  for (int i = (int)(kVertexStage + 1); i < kRenderPipelineStageNum; ++i) {
    RenderPipelineStage stage = (RenderPipelineStage)i;
    const TechniqueParser::StageInfo& shader = tech.shader[i];
    if (shader.entry == NULL) { continue; }
    ss << "  {\n"
       << "    const std::string& source = sources_[" << StageName(stage) << "];\n"
       << "    DCHECK(!source.empty());\n"
       << "    azer::GpuProgramPtr gpup_ptr(render_system_->CreateGpuProgram(\n"
       << "        " << StageName(stage) << ", source));\n"
       << "    DCHECK(gpup_ptr.get() != NULL);\n"
       << "    technique_->AddGpuProgram(gpup_ptr);\n"
       << "  }\n";
  }
  ss << "  technique_->Use(render_system_->GetDefaultRenderer());\n"
     << "}\n\n";
  return ss.str();
}

StructDeclNode* CppCodeGen::GetVertexDecl(const TechniqueParser::Technique& tech) {
  const TechniqueParser::StageInfo& stageinfo = tech.shader[kVertexStage];
  DCHECK(stageinfo.entry != NULL);
  DCHECK(stageinfo.entry->IsFuncDefNode());
  FuncProtoNode* proto = stageinfo.entry->ToFuncDefNode()->GetProtoNode();
  DCHECK(proto != NULL);
  DCHECK_EQ(proto->GetParams().size(), 1u);
  DCHECK(proto->GetParams().at(0)->IsParamNode());
  ParamNode* param = proto->GetParams().at(0)->ToParamNode();
  TypedNode* typed = param->GetTypedNode();
  DCHECK(typed != NULL && typed->GetStructDecl() != NULL);
  StructDeclNode* decl = typed->GetStructDecl();
  return decl;
}

std::string CppCodeGen::GenVertexStruct(const TechniqueParser::Technique& tech) {
  StructDeclNode* decl = GetVertexDecl(tech);

  // if vertex is extend, then return directly
  if (IsExternCppStruct(decl)) {
    return "";
  }

  std::stringstream ss;
  ss << "\n"
     << "  /**\n"
     << "  * struct of Vertex \n"
     << "  * input of Vertex Shader\n"
     << "  */\n"
     << "  struct Vertex {\n";
  for (auto iter = decl->fields().begin(); iter != decl->fields().end(); ++iter) {
    DCHECK((*iter)->IsFieldNode());
    FieldNode* struct_field = (*iter)->ToFieldNode();
    ss << "    " << UniformTypeName(struct_field) << " "
       << struct_field->fieldname() << ";\n";
  }
  ss << "    Vertex(){}\n"
     << "    Vertex(";
  int cnt = 0;
  for (auto iter = decl->fields().begin(); iter != decl->fields().end();
       ++iter, ++cnt) {
    if (iter != decl->fields().begin()) ss << ", ";
    ss << "const " << UniformTypeName((*iter)) << " p" << cnt;
  }
  ss << ")\n";
  cnt = 0;
  for (auto iter = decl->fields().begin(); iter != decl->fields().end();
       ++iter, ++cnt) {
    DCHECK((*iter)->IsFieldNode());
    std::string fieldname = (*iter)->ToFieldNode()->fieldname();
    if (cnt == 0) ss << "      : ";
    else ss <<"      , ";
    ss << fieldname << "(p" << cnt << ")\n";
  }
  ss << "      {}\n";
  ss << "  };\n";
  return ss.str();
}

std::string CppCodeGen::GenVertexDesc(const TechniqueParser::Technique& tech) {
  const TechniqueParser::StageInfo& stageinfo = tech.shader[kVertexStage];
  DCHECK(stageinfo.entry != NULL);
  DCHECK(stageinfo.entry->IsFuncDefNode());
  FuncProtoNode* proto = stageinfo.entry->ToFuncDefNode()->GetProtoNode();
  DCHECK(proto != NULL);
  DCHECK_EQ(proto->GetParams().size(), 1u);
  DCHECK(proto->GetParams().at(0)->IsParamNode());
  ParamNode* param = proto->GetParams().at(0)->ToParamNode();
  TypedNode* typed = param->GetTypedNode();
  DCHECK(typed != NULL && typed->GetStructDecl());
  StructDeclNode* decl = typed->GetStructDecl();

  std::stringstream ss;
  ss << "const azer::VertexDesc::Desc " << GetClassName(tech)
     << "::kVertexDesc[] = {\n";
  std::string prev_name;
  for (auto iter = decl->fields().begin(); iter != decl->fields().end(); ++iter) {
    DCHECK((*iter)->IsFieldNode());
    int semantic_index = GetSemanticIndex((*iter)->ToFieldNode());
    std::string fieldname = GetSemanticName((*iter)->ToFieldNode()->fieldname());
    ss << "  {\"" << StringToUpperASCII(fieldname)
       << "\", " << semantic_index << ", "
       << UniformTypeIndex((*iter)->ToFieldNode()->GetType()) << "},\n";
    prev_name = fieldname;
  }
  ss << "};\n";
  return ss.str();
}

std::string CppCodeGen::GenGpuTableInit(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  int cnt = 0;
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter, ++cnt) {
    if (iter->entry == NULL) continue;
    RenderPipelineStage stage = (RenderPipelineStage)cnt;
    ss << std::move(GenStageGpuTable(stage, iter->uniforms));
  }

  return ss.str();
}

std::string CppCodeGen::GenInit(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  ss << "void " << GetClassName(tech) << "::Init() {\n"
     << "  InitTechnique();\n"
     << std::move(GenGpuTableInit(tech))
     << "}\n\n";
  return ss.str();
}

std::string CppCodeGen::GenStageUniformStructure(RenderPipelineStage stage,
                                                 const ASTNodeVec& unideps) const {
  std::stringstream ss;
  std::vector<StructDeclNode*> decl_nodes;
  for (auto iter = unideps.begin(); iter != unideps.end(); ++iter) {
    DCHECK((*iter)->IsStructDeclNode());
    StructDeclNode* decl = (*iter)->ToStructDeclNode();
    if (!IsExternCppStruct(decl)) {
      decl_nodes.push_back(decl);
    }
  }
  if (unideps.size() == 0) { return "";}

  for (auto iter = decl_nodes.begin(); iter != decl_nodes.end(); ++iter) {
    DCHECK((*iter)->IsStructDeclNode());
    StructDeclNode* decl = (*iter);
    ss << "  struct " << SelfDefStructureName(*iter) << "{\n";
    for (auto field_iter = decl->fields().begin();
         field_iter != decl->fields().end(); ++field_iter) {
      FieldNode* field = (*field_iter);
      ss << "    " << UniformTypeName(field) << " "
         << field->fieldname() << ";\n";
    }
    ss << "  };\n";
  }
  return ss.str();
}

std::string CppCodeGen::GenStageExchangeBuffer(RenderPipelineStage s,
                                               const ASTNodeVec& uniforms) const {
  std::stringstream ss;
  ss << "  struct " << StagePrefix(s) << "_cbuffer {\n";
  for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
    DCHECK((*iter)->IsDeclarationNode());
    DeclarationNode* decl = (*iter)->ToDeclarationNode();
    ss << "    " << UniformTypeName(decl) << " ";
    ASTNode* cur = decl->first_child();
    if (cur->IsAttributesNode()) {cur = cur->next_sibling();}
    while (cur) {
      DCHECK (cur->IsSymbolNode());
      SymbolNode* symbol = cur->ToSymbolNode();
      TypePtr symbol_type = symbol->GetType();
      ss << symbol->symbolname() << DumpArraySpecifier(symbol_type);
        
      if (cur->next_sibling()) ss << ",";
      cur = cur->next_sibling();
    }
    ss << ";\n";
  }
  ss << "  };";
  return ss.str();
}

std::string CppCodeGen::GenExchangeBuffer(const TechniqueParser::Technique& tech)
    const {
  std::stringstream ss;
  ss << " /**" << std::endl
     << "   * uniform buffer\n"
     << "   */\n"
     << "  #pragma pack(push, 4)\n";
  int cnt = 0;
  for (auto tech_iter = tech.shader.begin();
       tech_iter != tech.shader.end(); ++tech_iter, ++cnt) {
    const TechniqueParser::StageInfo& stage = *tech_iter;
    if (stage.entry == NULL) {
      continue;
    }

    if (stage.uniforms.size() == 0u) continue;
    RenderPipelineStage s = (RenderPipelineStage)cnt;
    ss << std::move(GenStageUniformStructure(s, stage.uni_depend)) << "\n";
    ss << std::move(GenStageExchangeBuffer(s, stage.uniforms)) << "\n";
  }
  ss << "  #pragma pack(pop)" << std::endl;
  return ss.str();
}

void CppCodeGen::GenCppCode(const TechniqueParser::Technique& tech) {
  cpp_code_.clear();
  std::string classname = std::move(GetClassName(tech));
  std::stringstream ss;
  ss << " /**" << std::endl
     << " * Generated by afx\n"
     << " * dont edit by hande\n"
     << " */" << std::endl
     << "#include <stddef.h>\n\n"
     << "#include \"base/basictypes.h\"\n"
     << "#include \"base/logging.h\"\n\n"
     << "#include \"azer/render/render.h\"\n\n"
     << GenVertexDesc(tech) << "\n"
     << "const int " << classname << "::kVertexDescNum = arraysize("
     << classname << "::kVertexDesc);\n\n"
     << classname << "::" << classname
     << "(const std::vector<std::string>& sources, "
     << "azer::RenderSystem* rs) \n"
     << "  : azer::Effect(rs) \n"
     << "  , sources_(sources) {\n"
     << "  DCHECK(sources.size() == azer::kRenderPipelineStageNum);\n"
     << "  DCHECK(!sources[azer::kVertexStage].empty());\n"
     << "  DCHECK(!sources[azer::kPixelStage].empty());\n"
     << "  Init();\n"
     << "}\n\n"
     << classname << "::~" << classname << "() {\n"
     << "}\n\n"
     << std::move(GenInit(tech))
     << std::move(GenTechnique(tech))
     << GenUseTexture(tech) << "\n"
     << GenSetMaterial(tech) << "\n";
     
  cpp_code_ = std::move(ss.str());
}

void CppCodeGen::GenCode(const TechniqueParser::Technique& tech) {
  GenHeadCode(tech);
  GenCppCode(tech);
}

std::string CppCodeGen::GetClassName(const TechniqueParser::Technique& tech) const {
  std::string classname;
  if (tech.attributes) {
    if (tech.attributes->HasAttr("class_name")) {
      classname = tech.attributes->GetAttrValue("class_name");
    } else {
      classname = tech.name;
    }
  }
  return classname;
}

namespace {
void GetAllDeclNode(std::vector<StructDeclNode*> *nodes,
                    const ASTNodeVec& uniforms) {
  for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
    DCHECK((*iter)->IsStructDeclNode());
    StructDeclNode* decl = (*iter)->ToStructDeclNode();
    nodes->push_back(decl);
  }
}
}

std::string CppCodeGen::GenStructDepIncludeCode(
    const TechniqueParser::Technique& tech) {
  std::vector<StructDeclNode*> decl_nodes;
  std::set<std::string> includes;
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter) {
    const TechniqueParser::StageInfo& stage = *iter;
    GetAllDeclNode(&decl_nodes, stage.uni_depend);
  }

  for (auto iter = decl_nodes.begin(); iter != decl_nodes.end(); ++iter) {
    StructDeclNode* decl = (*iter)->ToStructDeclNode();
    if (IsExternCppStruct(decl)) {
      std::string v = decl->attributes()->GetAttrValue("cpphead");
      if (!v.empty()) {
        includes.insert(v);
      }
    }
  }

  StructDeclNode* decl = GetVertexDecl(tech);
  if (IsExternCppStruct(decl)) {
    std::string v = decl->attributes()->GetAttrValue("cpphead");
    if (!v.empty()) {
      includes.insert(v);
    }
  }

  std::stringstream ss;
  for (auto iter = includes.begin(); iter != includes.end(); ++iter) {
    ss << "#include \"" << *iter << "\"" << std::endl;
  }

  return ss.str();
}

void CppCodeGen::GenHeadCode(const TechniqueParser::Technique& tech) {
  head_code_.clear();
  std::stringstream ss;
  ss << " /**" << std::endl
     << " * Generated by afx\n"
     << " * dont edit by hande\n"
     << " */" << std::endl
     << "#pragma once\n" << std::endl
     << "#include \"azer/render/render.h\"" << std::endl
     << "#include \"azer/math/math.h\"" << std::endl
     << GenStructDepIncludeCode(tech) << std::endl
     << std::endl << std::endl;

  std::string classname = std::move(GetClassName(tech));
  ss << "class " << classname << ": public azer::Effect {\n"
     << " public:\n"
     << "  " << classname << "(const std::vector<std::string>& sources, "
     << "azer::RenderSystem* rs);\n"
     << "  ~" << classname << "();\n"
     << std::move(GenExchangeBuffer(tech)) << "\n"
     << std::move(GenUniformFuncs(tech)) << "\n"
     << std::move(GenVertexStruct(tech)) << "\n"
     << "  azer::VertexDescPtr GetVertexDesc() { return vertex_desc_ptr_;}\n"
     << "  static const int kVertexDescNum;\n"
     << "  static const azer::VertexDesc::Desc kVertexDesc[];\n"
     << " protected:\n"
     << "  void Init();\n"
     << "  void InitTechnique();\n"
     << "  virtual void UseTexture(azer::Renderer* renderer) OVERRIDE;\n"
     << "  virtual void SetMaterial(azer::MaterialPtr material) OVERRIDE;\n"
     << "  const std::vector<std::string>& sources_;\n\n"
     << GenAllTextureMember(tech) << "\n"
     << "  azer::VertexDescPtr vertex_desc_ptr_;\n"
     << "  DISALLOW_COPY_AND_ASSIGN(" << classname << ");\n"
     << "};\n";
  head_code_ = std::move(ss.str());
}

std::string CppCodeGen::GenTextureStageUse(RenderPipelineStage stage,
                                           const ASTNodeVec& textures) {
  std::stringstream ss;
  int index = 0;
  for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
    std::string varname = GetTexVarName(*iter, stage);
    if (IsTextureArray(*iter)) {
      ss << "  for (size_t i = 0; i < arraysize(" << varname << "); ++i) {\n"
         << "    if (" << varname << "[i].get() == NULL) { break; }\n"
         << "    renderer->UseTexture(" << StageName(stage) << ", i + " << index
         << ", " << varname << "[i].get());\n"
         << "  }\n";

      TypedNode* typed = GetTexNode(*iter);
      DCHECK(typed != NULL);
      TypePtr typeptr = typed->GetType();
      index += typeptr->GetDim(0);
    } else {
      ss << "  renderer->UseTexture(" << StageName(stage) << ", " << index++
         << ", " << varname << ".get());\n";
    }
  }
  return ss.str();
}

std::string CppCodeGen::GenUseTexture(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  std::string classname = std::move(GetClassName(tech));
  ss << "void " << classname << "::UseTexture(azer::Renderer* renderer) {\n";
  int cnt = 0;
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter, ++cnt) {
    if (iter->entry == NULL) continue;
    RenderPipelineStage stage = (RenderPipelineStage)cnt;
    ss << std::move(GenTextureStageUse(stage, iter->uni_textures));
  }
  
  ss << "}";
  return ss.str();
}

std::string CppCodeGen::GenAllTextureMember(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  int cnt = 0;
  for (auto iter = tech.shader.begin(); iter != tech.shader.end(); ++iter, ++cnt) {
    if (iter->entry == NULL) continue;
    RenderPipelineStage stage = (RenderPipelineStage)cnt;
    ss << std::move(GenStageTextureMember(stage, iter->uni_textures));
  }
  return ss.str();
}

std::string CppCodeGen::GenStageTextureMember(RenderPipelineStage stage,
                                              const ASTNodeVec& textures) {
  std::stringstream ss;
  for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
    ASTNode* node = *iter;
    ss << "  azer::TexturePtr " << GetTexVarName(*iter, stage)
       << HLSLTextureArraySpecifier(*iter) << ";\n";
  }
  return ss.str();
}

std::string CppCodeGen::GenSetMaterial(const TechniqueParser::Technique& tech) {
  std::stringstream ss;
  ss << "void " << GetClassName(tech)
     << "::SetMaterial(azer::MaterialPtr material) {\n";
  
  ss << "}\n\n";
  return ss.str();
}

namespace {
const char* StageName(RenderPipelineStage stage) {
  switch (stage) {
    case kInputAssemblerStage: return "azer::kInputAssemblerStage";
    case kVertexStage: return "azer::kVertexStage";
    case kHullStage: return "azer::kHullStage";
    case kTessellatorStage: return "azer::kTessellatorStage";
    case kDomainStage: return "azer::kDomainStage";
    case kGeometryStage: return "azer::kGeometryStage";
    case kRasterizerStage: return "azer::kRasterizerStage";
    case kPixelStage: return "azer::kPixelStage";
    case kOutputMergerStage: return "azer::kOutputMergerStage";
    case kRenderPipelineStageNum: return "azer::kRenderPipelineStageNum";
    default: NOTREACHED(); return "";
  }
}

const char* StagePrefix(RenderPipelineStage stage) {
  switch (stage) {
    case azer::kVertexStage: return "vs";
    case azer::kHullStage: return "hs";
    case azer::kTessellatorStage: return "ts";
    case azer::kDomainStage: return "ds";
    case azer::kGeometryStage: return "gs";
    case azer::kPixelStage: return "ps";
    default: NOTREACHED() << "No such RenderPipelineStage: " << stage; return "";
  }
}

// used by generate vertex desc
std::string UniformTypeIndex(const TypePtr& type) {
  switch(type->type()) {
    case kBoolean: return "kBool";
    case kShort: return "azer::kShort";
    case kInt32: return "kInt";
    case kUint32: return "kUint";
    case kFloat: return "kFloat";
    case kIntVec2: return "azer::kIntVec2";
    case kIntVec3: return "azer::kIntVec3";
    case kIntVec4: return "azer::kIntVec4";
    case kVector2: return "azer::kVec2";
    case kVector3: return "azer::kVec3";
    case kVector4: return "azer::kVec4";
    case kMatrix2: return "azer::kMat2";
    case kMatrix3: return "azer::kMat3";
    case kMatrix4: return "azer::kMat4";
    case kStructure:
      if (type->name() == "afx::DirectionalLight") {
        return "azer::Light";
      } else if (type->name() == "afx::PointLight") {
        return "azer::Light";
      } else if (type->name() == "afx::SpotLight") {
        return "azer::Light";
      } else if (type->name() == "afx::Material") {
        return "azer::Material::material";
      } else {
        NOTREACHED(); return "";
      }
    case kTexture1D:
    case kTexture2D:
    case kTexture3D:
    case kTextureCube:
    case kChar:
    case kString:
    case kDouble:
    case kAnonymouseStructure: 
    case kFunctional: 
    default: NOTREACHED(); return "";
  }
}

std::string UniformTypeName(ASTNode* node) {
  TypePtr& type = GetNodeType(node);
  switch(type->type()) {
    case kShort: return "short";
    case kInt32: return "int32";
    case kUint32: return "uint32";
    case kBoolean: return "bool";
    case kFloat: return "float";
    case kIntVec2: return "azer::IntVec2";
    case kIntVec3: return "azer::IntVec3";
    case kIntVec4: return "azer::IntVec4";
    case kVector2: return "azer::Vector2";
    case kVector3: return "azer::Vector3";
    case kVector4: return "azer::Vector4";
    case kMatrix2: return "azer::Matrix2";
    case kMatrix3: return "azer::Matrix3";
    case kMatrix4: return "azer::Matrix4";
    case kTexture1D: return "azer::Texture";
    case kTexture2D: return "azer::Texture";
    case kTexture3D: return "azer::Texture";
    case kTextureCube: return "azer::Texture";
    case kStructure: {
      
      std::string output_name = GetStructTypeName(node);
      if (output_name.empty()) {
        ::base::ReplaceChars(type->name(), "::", "__", &output_name);
      }
      return output_name;
    }
    case kChar:
    case kString:
    case kDouble:
    case kAnonymouseStructure: 
    case kFunctional: 
    default: NOTREACHED(); return "";
  }
}

std::string GetStructTypeName(ASTNode* node) {
  if (node->IsStructDeclNode()) {
    StructDeclNode* decl = node->ToStructDeclNode();
    if (IsExternCppStruct(decl)) {
      return decl->attributes()->GetAttrValue("cppstruct");
    }
  } else if (node->IsDeclarationNode()) {
    DeclarationNode* declare = node->ToDeclarationNode();
    TypedNode* tnode = declare->GetTypedNode();
    DCHECK(tnode != NULL && tnode->GetStructDecl());
    StructDeclNode* decl = tnode->GetStructDecl()->ToStructDeclNode();
    if (IsExternCppStruct(decl)) {
      return decl->attributes()->GetAttrValue("cppstruct");
    }
  }
  return "";
}
  

inline std::string GpuTableDescVarName(RenderPipelineStage stage) {
  std::string ret = std::string(StagePrefix(stage)) + "_table_desc";
  return ret;
}

inline bool IsSelfDefinedStruct(const TypePtr& typeptr) {
  if (typeptr->IsStructure()) {
    if ((typeptr->name() == "afx::DirectionalLight")
        || (typeptr->name() == "afx::PointLight")
        || (typeptr->name() == "afx::SpotLight")
        || (typeptr->name() == "afx::Material")) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;
  }
}

const char* GpuConstantTypeFromUniform(const TypePtr& typeptr) {
  if (typeptr->IsStructure()) {
    /** 内置类型 */
    if (typeptr->name() == "afx::DirectionalLight") {
      return "azer::GpuConstantsType::kDirectionalLight";
    } else if (typeptr->name() == "afx::PointLight") {
      return "azer::GpuConstantsType::kPointLight";
    } else if (typeptr->name() == "afx::SpotLight") {
      return "azer::GpuConstantsType::kSpotLight";
    } else if (typeptr->name() == "afx::Material") {
      return "azer::GpuConstantsType::kMaterial";
    } else {
      NOTREACHED();
      return "";
    }
  } else {
    switch (typeptr->type()) {
      case kInt32: return "azer::GpuConstantsType::kInt";
      case kUint32: return "azer::GpuConstantsType::kUint";
      case kBoolean: return "azer::GpuConstantsType::kBool";
      case kFloat: return "azer::GpuConstantsType::kFloat";
      case kVector2: return "azer::GpuConstantsType::kVector2";
      case kVector3: return "azer::GpuConstantsType::kVector3";
      case kVector4: return "azer::GpuConstantsType::kVector4";
      case kMatrix2: return "azer::GpuConstantsType::kMatrix2";
      case kMatrix3: return "azer::GpuConstantsType::kMatrix3";
      case kMatrix4: return "azer::GpuConstantsType::kMatrix4";
      case kTexture1D:
      case kTexture2D:
      case kTexture3D:
      case kTextureCube:
      default:
        NOTREACHED() << "TODO";
        return "";
    }
  }

  NOTREACHED() << "TODO";
  return "";
}

std::string GetTexVarName(ASTNode* node, RenderPipelineStage stage) {
  std::stringstream ss;
  ss << StagePrefix(stage) << "_" << GenReferredTextureVarName(node) << "_";
  return ss.str();
}

std::string SelfDefStructureName(StructDeclNode* node) {
  std::stringstream ss;
  if (!node->GetContext()->package().empty()) {
    ss << node->GetContext()->package() << "__";
  }

  ss << node->struct_name();
  return ss.str();
}

bool SplitSemantic(const std::string& str, std::string* sname,
                   std::string* sindex) {
  int semantic_index = str.length() - 1;
  for (auto iter = str.rbegin(); iter != str.rend(); ++iter) {
    if (isdigit(*iter)) {
      semantic_index--;
    } else {
      break;
    }
  }

  *sname = str.substr(0, semantic_index + 1);
  *sindex = str.substr(semantic_index + 1);
  return semantic_index != str.length() - 1;
}

std::string GetSemanticName(const std::string& name) {
  std::string sname, sindex;
  SplitSemantic(name, &sname, &sindex);
  return sname;
}

int GetSemanticIndex(FieldNode* field) {
  if (field->attributes()) {
    AttributesNode* attr = field->attributes();
    const std::string& value = attr->GetAttrValue("semantic_index");
    if (value.empty()) return 0;
    int index = 0;
    DCHECK(::base::StringToInt(value, &index));
    return index;
  } else {
    std::string name = field->fieldname();
    std::string sname, sindex;
    if (!SplitSemantic(name, &sname, &sindex)) {
      return 0;
    } else {
      int index = 0;
      CHECK(::base::StringToInt(sindex, &index));
      return index;
    }
  }
}

bool IsExternCppStruct(StructDeclNode* decl) {
  // check if the struct is declared in another cpp header
  return decl->attributes() && decl->attributes()->HasAttr("cppstruct");
}
}  // namespace
}  // namespace afx
}  // namespace azer
