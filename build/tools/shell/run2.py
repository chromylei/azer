import os
import sys
import subprocess

if __name__ == '__main__':
    newargs = []
    for arg in sys.argv[1:]:
        newargs.append(arg.replace("/", "\\"))
    os.sys.stderr.write("%s\n" % (sys.argv[1:]))
    #os.sys.stderr.write("%s\n" % (newargs))
    subprocess.call(newargs);
