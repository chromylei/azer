
import os;
import sys;

if __name__ == "__main__":
  sys.stderr.write("[Python Shell]: makedirs %s\n" % (sys.argv[1]))
  if not os.path.exists(sys.argv[1]):
    os.makedirs(sys.argv[1])
