#include "sandbox/azer/common/md5loader.h"

#include <iostream>
#include <d3d11.h>
#include <xnamath.h>

#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_tokenizer.h"
#include "azer/render/render.h"

const azer::VertexDesc::Desc MD5Loader::kVertexDescs[] = {
  {"POSITION", azer::VertexDesc::kPosition, azer::kVec4},
  {"TEXCOORD", azer::VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL",   azer::VertexDesc::kNormal,   azer::kVec3},
};

const int MD5Loader::kDescNum = (int32)arraysize(MD5Loader::kVertexDescs);

void MD5Loader::HandleLine(const base::StringPiece& line) {
  ::base::StringPiece slice(line);
  if (state_ == kIntermid) {
    HandleMetaInfo(slice);
  } else if (state_ == kMesh) {
    HandleMeshLine(line);
  } else if (state_ == kJoints) {
    HandleJointsLine(line);
  }
}

void MD5Loader::HandleJointsLine(const base::StringPiece& line) {
  if (line == "}") {
    state_ = kIntermid;
    return;
  }
  Joint joint;
  std::string str = line.as_string();
  base::StringTokenizer t(str, " \t");
  t.set_quote_chars("\"");
  CHECK(t.GetNext());
  strncpy(joint.name, t.token().c_str(), sizeof(joint.name));
  CHECK(t.GetNext());
  CHECK(::base::StringToInt(t.token(), &joint.parent));
  CHECK(t.GetNext()); CHECK_EQ(t.token(), "(");
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.pos[0]));
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.pos[2]));
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.pos[1]));
  CHECK(t.GetNext()); CHECK_EQ(t.token(), ")");
  CHECK(t.GetNext()); CHECK_EQ(t.token(), "(");
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.rot[0]));
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.rot[2]));
  CHECK(t.GetNext()); CHECK(::base::StringToFloat(t.token(), &joint.rot[1]));
  CHECK(t.GetNext()); CHECK_EQ(t.token(), ")");
  float w = 1.0f - joint.rot[0] * joint.rot[0]
      - joint.rot[1] * joint.rot[1]
      - joint.rot[2] * joint.rot[2];
  if (w < 0.0f) {
    joint.rot[3] = 0.0f;
  } else {
    joint.rot[3] = -sqrt(w);
  }
  joints_.push_back(joint);
}

void MD5Loader::HandleVertex(const std::string& data) {
  std::vector<std::string> item;
  base::SplitString(data, ' ', &item);

  Vertex vertex;
  int idx = 0;
  CHECK(::base::StringToInt(item[idx++], &vertex.index));
  CHECK(item[idx++] == "(") << item[idx-1];
  CHECK(::base::StringToFloat(item[idx++], &vertex.tex[0])) << item[idx-1];
  CHECK(::base::StringToFloat(item[idx++], &vertex.tex[1]))<< item[idx-1];
  CHECK(item[idx++] == ")");
  CHECK(::base::StringToInt(item[idx++], &vertex.weight_start));
  CHECK(::base::StringToInt(item[idx++], &vertex.weight_num));
  mesh_.back()->vertices.push_back(vertex);
}

void MD5Loader::HandleWeight(const std::string& data) {
  std::vector<std::string> item;
  base::SplitString(data, ' ', &item);

  Weight weight;
  int idx = 0;
  CHECK(::base::StringToInt(item[idx++], &weight.index));
  CHECK(::base::StringToInt(item[idx++], &weight.joint));
  CHECK(::base::StringToFloat(item[idx++], &weight.bias));
  CHECK(item[idx++] == "(") << item[idx-1];
  CHECK(::base::StringToFloat(item[idx++], &weight.pos[0])) << item[idx-1];
  CHECK(::base::StringToFloat(item[idx++], &weight.pos[2]))<< item[idx-1];
  CHECK(::base::StringToFloat(item[idx++], &weight.pos[1]));
  CHECK(item[idx++] == ")");
  
  mesh_.back()->weights.push_back(weight);
}

void MD5Loader::HandleMeshLine(const base::StringPiece& line) {
  if (line == "}") {
    state_ = kIntermid;
    return;
  }

  std::vector<std::string> vec;
  ::base::SplitString(line.as_string(), ' ', &vec);
  if (vec[0] == "shader") {
    CHECK(mesh_.back()->tex.length() == 0u);
    mesh_.back()->tex = line.as_string().substr(strlen("shader "));
  } else if (vec[0] == "numverts") {
  } else if (vec[0] == "vert") {
    HandleVertex(line.as_string().substr(strlen("vert ")));
  } else if (vec[0] == "tri") {
    Triangle tri;
    std::vector<std::string> item;
    base::SplitString(line.as_string().substr(strlen("tri ")), ' ', &item);
    CHECK(::base::StringToInt(item[0], &tri.index));
    CHECK(::base::StringToInt(item[1], &(tri.data[0])));
    CHECK(::base::StringToInt(item[2], &(tri.data[1])));
    CHECK(::base::StringToInt(item[3], &(tri.data[2])));
    mesh_.back()->triangles.push_back(tri);
  } else if (vec[0] == "weight") {
    HandleWeight(line.as_string().substr(strlen("weight ")));
  }  
}

void MD5Loader::HandleMetaInfo(const base::StringPiece& line) {
  if (line.starts_with("numJoints ")) {
    /*
    int n = 0;
    CHECK(base::StringToInt(line.substr(strlen("numJoints ")), &n));
    joints_.resize(n);
    */
  } else if (line.starts_with("numMeshes ")) {
  } else if (line.starts_with("MD5Version ")) {
  } else if (line.starts_with("commandline")) {
  } else if (line.starts_with("mesh {")) {
    state_ = kMesh;
    Mesh* mesh = new Mesh;
    mesh_.push_back(mesh);
  } else if (line.starts_with("joints {")) {
    state_ = kJoints;
  }
}

void MD5Loader::ComputeNormal(VBVertex* v, int vertex_num, int32* indices,
                              int indices_num) {
  std::unique_ptr<uint32> used(new uint32[vertex_num]);
  memset(used.get(), 0, sizeof(uint32) * vertex_num);
  for (int i = 0; i < indices_num; i+=3) {
    VBVertex* p1 = v + indices[i];
    VBVertex* p2 = v +indices[i+1];
    VBVertex* p3 = v + indices[i+2];
    used.get()[indices[i]]++;
    used.get()[indices[i+1]]++;
    used.get()[indices[i+2]]++;
    azer::Vector3 normal = azer::CalcPlaneNormal(p1->pos, p2->pos, p3->pos);
    p1->normal += normal;
    p2->normal += normal;
    p3->normal += normal;
  }

  for (int i = 0; i < vertex_num; ++i) {
    v[i].normal /= (float)used.get()[i];
  }
}

azer::Vector3 MD5Loader::CalcPos(Mesh* mesh, const Vertex& v) {
  azer::Vector3 pos;
  for (int i = 0; i < v.weight_num; ++i) {
    const int widx = v.weight_start + i;
    const Weight& weight = mesh->weights[widx];
    const Joint& joint = joints_[weight.joint];

    pos += azer::Vector3(joint.pos[0], joint.pos[1], joint.pos[2]) * weight.bias;
    XMVECTOR weightpos = XMVectorSet(weight.pos[0], weight.pos[1], weight.pos[2], 0.0f);
    XMVECTOR rpos = XMVectorSet(joint.rot[0], joint.rot[1], joint.rot[2], 0.0f);
    XMVECTOR roi = XMVectorSet(joint.rot[0], joint.rot[1], joint.rot[2],
                               joint.rot[3]);
    XMVECTOR roi_conj = XMVectorSet(-joint.rot[0], -joint.rot[1], -joint.rot[2],
                                    joint.rot[3]);
    XMFLOAT3 pt;
    XMStoreFloat3(&pt, XMQuaternionMultiply(
        XMQuaternionMultiply(roi, weightpos),roi_conj));
    pos += azer::Vector3(pt.x, pt.y, pt.z) * weight.bias;
  }

  return pos;
}

void MD5Loader::CreateMesh(azer::Mesh* amesh) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  for (auto iter = mesh_.begin(); iter != mesh_.end(); ++iter) {
    int cnt = 0;
    Mesh* mesh = *iter;
    azer::SubMeshPtr sub(new azer::Mesh::SubMesh);
    azer::VertexDescPtr vbdesc(new azer::VertexDesc(kVertexDescs,
                                                    ARRAYSIZE(kVertexDescs)));
    azer::VertexDataPtr vbd(new azer::VertexData(vbdesc, mesh->vertices.size()));
    VBVertex* vptr_begin = (VBVertex*)vbd->pointer();
    memset(vptr_begin, 0, sizeof(VBVertex) * mesh->vertices.size());
    VBVertex* vptr = vptr_begin;
    for (auto viter = mesh->vertices.begin(); viter != mesh->vertices.end();
         ++viter, ++cnt) {
      vptr->pos = azer::Vector4(CalcPos(mesh, *viter), 1.0f);
      vptr->tex = azer::vec2((float)viter->tex[0], (float)viter->tex[1]);
      vptr++;
    }

    // compute index 
    azer::IndicesDataPtr ibd(new azer::IndicesData(mesh->triangles.size() * 3,
                                                   azer::IndicesData::kUint32,
                                                   azer::IndicesData::kMainMemory));
    uint32* iptr_begin = (uint32*)ibd->pointer();
    uint32* iptr = iptr_begin;
    for (auto titer = mesh->triangles.begin();
         titer != mesh->triangles.end(); ++titer) {
      *iptr++ = titer->data[0];
      *iptr++ = titer->data[1];
      *iptr++ = titer->data[2];
    }

    ComputeNormal((VBVertex*)vbd->pointer(), mesh->vertices.size(),
                  (int32*)ibd->pointer(), mesh->triangles.size() * 3);

    sub->vb_ptr.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), vbd));
    sub->ib_ptr.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), ibd));

    //
    azer::Mesh::Group group;
    group.first = 0; group.num = mesh->triangles.size() * 3;
    if (!mesh->tex.empty()) {
      std::string s;
      base::TrimString(mesh->tex, "\"", &s);
      std::wstring path = UTF8ToWide(s);
      std::wstring str = path_.DirName().value() + TEXT("/") + path;
      LOG(ERROR) << "load texture : " << str;
      azer::TexturePtr tex(rs->CreateTextureFromFile(
          azer::Texture::k2D, base::FilePath(str)));
      group.mtrl_ptr.reset(new azer::Material);
      group.mtrl_ptr->AddTexture(tex);
    }

    sub->groups.push_back(group);
    amesh->Add(sub);
  }
}

bool MD5Loader::Load(const base::FilePath& path) {
  path_ = path;
  std::string content;
  if (!base::ReadFileToString(path, &content)) {
    return false;
  }
  std::vector<std::string> vec;
  base::SplitString(content, '\n', &vec);
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    std::string str = *iter, trim;
    int pos = str.find("//");
    if (pos != base::StringPiece::npos) {
      str = str.substr(0, pos);
    }
    TrimWhitespace(str, TRIM_ALL, &trim);
    if (trim.length() > 0) {
    ::base::StringPiece slice(trim);
      HandleLine(slice);
    }
  }

  return true;
}

void MD5Loader::dump() {
  for (auto iter = mesh_.begin(); iter != mesh_.end(); ++iter) {
    std::cout << "mesh {";
    for (auto viter = (*iter)->vertices.begin();
         viter != (*iter)->vertices.end(); ++viter) {
      std::cout << viter->index << " "
                << "(" << viter->tex[0] << ", " << viter->tex[1] << ") "
                << viter->weight_start << " " << viter->weight_num << std::endl;
    }
    std::cout << "}" << std::endl << std::endl;
  }
}
