import os
import sys
import subprocess

if __name__ == '__main__':
    os.sys.stderr.write("%s\n" % (sys.argv[1:]))
    subprocess.call(sys.argv[1:])
