#!/usr/bin/env python3

import re
import sys

def main(argv):

    log_entries_in_file = sys.argv[1]
    log_entries_header_file = sys.argv[2]

    print("Log entries input file:", log_entries_in_file)
    print("Log entries output file:", log_entries_header_file)

    with open(log_entries_in_file) as in_file:
        lines = in_file.readlines()

    with open(log_entries_header_file, 'w') as header_file:
        header_file.write("#pragma once\n\n")
        for line in lines:
            match = re.search(r'([A-Z_]*) (\"[A-Za-z ]*\")', line)
            if match:
                header_file.write("#define " + match.groups()[0] + " " + match.groups()[1] + "\n")
        header_file.close()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
