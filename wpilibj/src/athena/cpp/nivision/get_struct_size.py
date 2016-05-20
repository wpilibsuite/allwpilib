from __future__ import print_function

import sys


def main():
    for line in sys.stdin:
        line = line.strip()
        if not line.startswith("#STRUCT_SIZER"):
            continue
        line = line[14:]
        line = line.replace("#", "")
        print(line)


if __name__ == "__main__":
    main()
