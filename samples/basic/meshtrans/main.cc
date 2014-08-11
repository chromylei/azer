#include <vector>

#include "azer/azer.h"
#include "azer/resources/mesh.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_number_conversions.h"

using azer::detail::MeshHead;
using azer::detail::GroupAttr;
using azer::VertexDataPtr;
using azer::VertexData;

struct Vertex {
  azer::Vector3 position;
  azer::Vector2 texcoord;
  azer::Vector3 normal;
  Vertex(const azer::Vector3& p, const azer::Vector2& t, const azer::Vector3& n)
      : position(p)
      , texcoord(t)
      , normal(n) {}
};

std::vector<Vertex> loadModel(const base::FilePath& path);

std::string FLAG_vertex_desc;
std::string FLAG_material;
std::string FLAG_input;
std::string FLAG_output;
std::string FLAG_resource_root;
int ParseArgs();

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) OVERRIDE {}; 
  virtual void OnRenderScene(double time, float delta_time) OVERRIDE {};
  virtual void OnQuit() OVERRIDE {};
};

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  if (0 != ParseArgs()) {
    return -1;
  }

  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();

  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();

  // load data from file
  std::vector<Vertex> vertices = std::move(
      loadModel(base::FilePath(::base::UTF8ToWide(FLAG_input))));

  // create resource mananger and get vertex_desc
  azer::FileSystemPtr sample_res_fs;
  std::unique_ptr<azer::ResourceManager> res_mgr;
  sample_res_fs.reset(azer::FileSystem::create(
      azer::FileSystem::kNativeFS, ::base::FilePath(
          ::base::UTF8ToWide(FLAG_resource_root))));
  res_mgr.reset(new azer::ResourceManager(sample_res_fs));

  azer::VertexDescPtr vertex_desc_ptr = res_mgr->GetVertexDesc(
      ::base::UTF8ToWide(FLAG_vertex_desc));
  VertexDataPtr vertex_data(new VertexData(vertex_desc_ptr, vertices.size()));
  memcpy(vertex_data->pointer(), &vertices[0], vertices.size() * sizeof(Vertex));

  azer::AxisAlignedBox aabb =
      std::move(azer::AxisAlignedBox::CalcForVertexData(*vertex_data.get()));

  uint32 total_size = sizeof(MeshHead) + sizeof(GroupAttr)
      + sizeof(Vertex) * vertices.size();
  std::unique_ptr<uint8[]> data(new uint8[total_size]);
  memset(data.get(), 0, total_size);
  uint8* ptr = data.get();
  MeshHead* head = (MeshHead*)ptr;
  memcpy(head->ident, "afxm", 4);
  head->version = 0;
  strcpy(head->vertex_desc_name, FLAG_vertex_desc.c_str());
  head->vertex_unit_size = sizeof(Vertex);
  head->vertex_num = (int32)vertices.size();
  head->vertex_offset = sizeof(MeshHead) +  sizeof(GroupAttr);

  // write bounding volumn
  head->bounding_volumn_type = azer::BoundingVolumn::kAxisAlignedBox;
  float* bounding_data = (float*)head->bounding_volumn_data;
  bounding_data[0] = aabb.minimum().x;
  bounding_data[1] = aabb.minimum().y;
  bounding_data[2] = aabb.minimum().z;
  bounding_data[3] = aabb.minimum().w;
  bounding_data[4] = aabb.maximum().x;
  bounding_data[5] = aabb.maximum().y;
  bounding_data[6] = aabb.maximum().z;
  bounding_data[7] = aabb.maximum().w;

  head->group_num = 1;
  head->group_offset = sizeof(MeshHead);

  GroupAttr* groups = (GroupAttr*)(ptr + head->group_offset);
  groups->type = azer::Mesh::kVertex;
  groups->range.first = 0;
  groups->range.num = vertices.size();
  groups->primitive = azer::kTriangleList;
  strcpy(groups->mtrl, FLAG_material.c_str());
  memcpy(ptr + head->vertex_offset, &vertices[0], sizeof(Vertex) * vertices.size());

  return file_util::WriteFile(base::FilePath(::base::UTF8ToWide(FLAG_output)),
                              (const char*)data.get(), total_size);
}

float toFloat(const std::string& input) {
  float value;
  DCHECK(base::StringToFloat(input, &value));
  return value;
}

void split(const std::string& str, std::vector<std::string>* vec) {
  const char* input = str.c_str();
  ::base::CStringTokenizer t(input, input + str.length(), " ");
  while (t.GetNext()) {
    vec->push_back(t.token());
  }
}

std::vector<Vertex> loadModel(const base::FilePath& path) {
  std::vector<Vertex> ret;
  std::string content;
  std::vector<std::string> lines;
  CHECK(::base::ReadFileToString(path, &content)) << path.value().c_str();
  base::SplitString(content, '\n', &lines);
  for (auto iter = lines.begin(); iter != lines.end(); ++iter) {
    std::string str;
    TrimWhitespaceASCII(*iter, TRIM_ALL, &str);
    char c = str.c_str()[0];
    if ((c >= '0' && c <= '9') || c == '+' || c == '-') {
      std::vector<std::string> num;
      split(str, &num);
      DCHECK_EQ(num.size(), 8u) << str;
      Vertex vertex(
          azer::Vector3(toFloat(num[0]), toFloat(num[1]), toFloat(num[2])),
          azer::Vector2(toFloat(num[3]), toFloat(num[4])),
          azer::Vector3(toFloat(num[5]), toFloat(num[6]), toFloat(num[7])));
      ret.push_back(vertex);
    }
  }

  return ret;
}

int ParseArgs() {
  CommandLine* cmd = CommandLine::ForCurrentProcess();
  FLAG_vertex_desc = cmd->GetSwitchValueASCII("vertex_desc");
  FLAG_input = cmd->GetSwitchValueASCII("input");
  FLAG_output = cmd->GetSwitchValueASCII("output");
  FLAG_resource_root = cmd->GetSwitchValueASCII("resource_root");
  FLAG_material = cmd->GetSwitchValueASCII("material");
  if (FLAG_resource_root.empty()) {
    FLAG_resource_root = "samples/resources";
  }

  if (FLAG_vertex_desc.empty()) {
    FLAG_vertex_desc = "vdesc/base.vdesc:base1";
  }

  if (FLAG_material.empty()) {
    FLAG_material = "material/base.mtrl:base";
  }
  return 0;
}
