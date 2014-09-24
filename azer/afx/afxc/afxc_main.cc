#include <iostream>
#include <fstream>

#include "base/base.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/strings/string_util.h"
#include "base/strings/string_split.h"
#include "base/strings/stringprintf.h"
#include "azer/render/render.h"
#include "azer/afx/codegen/afx_codegen.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/codegen/cpp_codegen.h"
#include "azer/afx/codegen/util.h"
#include "azer/afx/linker/afx_parser.h"

int ParseArgs();
void PrintHelp();

bool FLAGS_hlslang = false;
bool FLAGS_glslang = false;
base::FilePath::StringType FLAGS_afxpath;
base::FilePath::StringType FLAGS_includes;
std::string FLAGS_output_dir;
std::string FLAGS_cpp_filename;

void GenHLSLTechniques(azer::afx::TechniqueParser* parser);
void GenCppCode(azer::afx::TechniqueParser* parser);
std::string stage_supfix(azer::RenderPipelineStage stage);

void WriteContent(const std::string& path, const std::string& content);

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "AfxCompiler");
  if (0 != ParseArgs()) {
    return -1;
  }

  std::vector<::base::FilePath::StringType> inc;
  ::base::SplitString(FLAGS_includes, FILE_PATH_LITERAL(','), &inc);
  azer::afx::AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser afx_parser(inc, opt);
  afx_parser.Parse(::base::FilePath(FLAGS_afxpath));
  if (!afx_parser.success()) {
    std::cerr << "Failed to compile afxfile: \""
              << FLAGS_afxpath << "\"" << std::endl;
    if (!afx_parser.GetCompileError().empty()) {
      std::cerr << "compiler error: \n" << afx_parser.GetCompileError() << std::endl;
    }
    if (!afx_parser.GetErrorText().empty()) {
      std::cerr << "link error: \n" << afx_parser.GetErrorText() << std::endl;
    }
    return -1;
  }

  if (FLAGS_hlslang) {
    std::cout << "generate hlslang code" << std::endl;
    GenHLSLTechniques(afx_parser.GetTechniques());
  }

  GenCppCode(afx_parser.GetTechniques());
  return 0;
}

void GenCppCode(azer::afx::TechniqueParser* parser) {
  using azer::afx::CppCodeGen;
  std::string hpppath = ::base::StringPrintf("%s/%s.afx.h",
                                             FLAGS_output_dir.c_str(),
                                             FLAGS_cpp_filename.c_str());
  std::string cpppath = ::base::StringPrintf("%s/%s.afx.cc",
                                             FLAGS_output_dir.c_str(),
                                             FLAGS_cpp_filename.c_str());
  for (auto iter = parser->GetTechniques().begin();
       iter != parser->GetTechniques().end(); ++iter) {
    CppCodeGen codegen;
    codegen.GenCode(iter->second);
    std::string hpp_code = codegen.GetHeadCode();
    std::string cpp_code = codegen.GetCppCode();
    WriteContent(hpppath, hpp_code);
    WriteContent(cpppath, cpp_code);
  }
}

void GenHLSLTechniques(azer::afx::TechniqueParser* parser) {
  using azer::afx::TechniqueParser;
  
  for (auto iter = parser->GetTechniques().begin();
       iter != parser->GetTechniques().end(); ++iter) {
    int cnt = 0;
    for (auto shader_iter = iter->second.shader.begin();
         shader_iter != iter->second.shader.end(); ++shader_iter, ++cnt) {
      if (!shader_iter->entry) continue;

      azer::RenderPipelineStage stage = (azer::RenderPipelineStage)cnt;
      std::stringstream ss;
      ss << FLAGS_output_dir << "/" << iter->second.name << ".afx."
         << stage_supfix((azer::RenderPipelineStage)stage) << "";
      std::string path = ss.str();
      
      azer::afx::HLSLCodeGeneratorFactory gen_factory;
      azer::afx::AfxCodegen codegen(&gen_factory);
      std::string code = azer::afx::FormatCode(codegen.GenCode(
          stage, *shader_iter, true));
      WriteContent(path, code);
    }
  }
}

std::string stage_supfix(azer::RenderPipelineStage stage) {
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

void Help() {
  std::cout << "afxc: transloate afx to hlsl or glslang, and generate "
            << " c++ wrapper class " << std::endl
            << "  --afx" << std::endl
            << "  --output_dir" << std::endl
            << "  --includes" << std::endl
            << "  --glslang: generate glslang code" << std::endl
            << "  --hlslang: generate hlslang code" << std::endl;
}

int ParseArgs() {
  CommandLine* cmd = CommandLine::ForCurrentProcess();
  if (cmd->HasSwitch("help")) {
    Help();
    return -1;
  }

  FLAGS_afxpath = cmd->GetSwitchValueNative("afx");
  FLAGS_output_dir = cmd->GetSwitchValueASCII("output_dir");
  FLAGS_includes = cmd->GetSwitchValueNative("includes");
  FLAGS_hlslang = cmd->HasSwitch("hlslang");
  FLAGS_glslang = cmd->HasSwitch("glslang");
  FLAGS_cpp_filename = cmd->GetSwitchValueASCII("cpp_filename");

  if (FLAGS_includes.empty()) {
    std::cerr << "includes cannot be empty\n";
    return -1;
  }

  if (FLAGS_afxpath.empty()) {
    std::cerr << "afx cannot be empty\n";
    return -1;
  }

  if (FLAGS_output_dir.empty()) {
    std::cerr << "output_dir cannot be empty\n";
    return -1;
  }

  if (FLAGS_cpp_filename.empty()) {
    std::cerr << "cpp_filename cannot be empty\n";
    return -1;
  }

  return 0;
}

void WriteContent(const std::string& path, const std::string& content) {
  ::base::FilePath afxpath(::base::UTF8ToWide(path));
  if (!PathExists(afxpath)) {
    if (::file_util::WriteFile(afxpath, content.c_str(), content.length())  == -1) {
      LOG(ERROR) << "failed to write file \"" << path << "\"";
    }
    return;
  }

  std::string tmp = path + ".afx.tmp";
  ::base::FilePath tmppath(::base::UTF8ToWide(tmp));
  if (::file_util::WriteFile(tmppath, content.c_str(), content.length())  == -1) {
    LOG(ERROR) << "failed to write file \"" << tmp << "\"";
  }

  if (TextContentsEqual(afxpath, tmppath)) {
    ::base::DeleteFile(tmppath, false);
  } else {
    ::base::PlatformFileError error;
    if (!::base::ReplaceFile(tmppath, afxpath, &error)) {
      PLOG(ERROR) << "failed to replace file \"" << tmp << "\"";
    }
  }
}
