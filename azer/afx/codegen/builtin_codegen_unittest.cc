#include "azer/afx/codegen/afx_codegen.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/linker/technique_parser.h"
#include "azer/afx/linker/linker.h"
#include "azer/afx/linker/testutil.h"
#include "azer/afx/linker/afx_parser.h"
#include "base/file_util.h"
#include "gtest/gtest.h"

using azer::afx::AfxLinker;
using azer::afx::AfxCodegen;
using azer::afx::ASTreeDumper;
using azer::afx::FileLoader;
using azer::afx::ParseContext;
using azer::afx::TechniqueParser;

const std::string kTestdataDir = "azer/afx/testdata";
const std::string kAfxDir = "azer/afx/afx";

TEST(BuiltMaterial, Base) {
}
