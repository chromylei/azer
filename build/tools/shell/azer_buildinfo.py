import os;
import sys;
import time;
import socket;

if __name__ == "__main__":
    gen_dir = sys.argv[1];
    buildinfo_file = sys.argv[2]
    absolute_build_path = os.path.dirname(os.path.abspath(__file__)) + "..//..//..//..";
    absolute_build_path = os.path.normpath(absolute_build_path);
    path_encoded = absolute_build_path.encode("string_escape");
    print sys.argv;
    if (not os.path.exists(os.path.dirname(gen_dir))):
        os.makedirs(os.path.dirname(gen_dir))

    lt = time.localtime();
    build_machine = socket.gethostname();
    build_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime());
    f = open("%s/%s.cc" % ( gen_dir, buildinfo_file), "w");
    f.write("#include \"%s.h\"\n\n" % (buildinfo_file))
    f.write("namespace base {\n\n");
    f.write("const int kBuildRootDirLength = %d;\n" % (len(absolute_build_path)));
    f.write("const char* kBuildRootDir = \"%s\";\n" % (path_encoded));
    f.write("const char* kBuildTime=\"%s\";\n" % (build_time))
    f.write("const char* kBuildMachine=\"%s\";\n" % (build_machine))
    f.write("}  // namespace base\n\n")

    f = open("%s/%s.h" % ( gen_dir, buildinfo_file), "w");
    f.write("#pragma once\n\n");
    f.write("namespace base {\n\n");
    f.write("extern const int kBuildRootDirLength;\n");
    f.write("extern const char* kBuildRootDir;\n");
    f.write("extern const char* kBuildTime;\n");
    f.write("extern const char* kBuildMachine;\n");
    f.write("}  // namespace base\n\n")
    
