#!/usr/bin/env python3

import re
import sys

def generate_header_file(log_entries, log_entries_header_file):
    print("Log entries header file:", log_entries_header_file)

    with open(log_entries_header_file, 'w') as header_file:
        header_file.write("#pragma once\n\n")
        for log_entry in log_entries:
            header_file.write("#define " + log_entry[0] + " " + log_entry[1] + "\n")
        header_file.close()

    return

def generate_messsages_file(log_entries, log_entries_messsages_file):
    print("Log entries messages file:", log_entries_messages_file)

    with open(log_entries_messages_file, 'w') as messages_file:
        header_file.write("messages -> LogStream NotRefCounted Stream {\n")
        for log_entry in log_entries:
            messages_file.write("    " + log_entry[0] + "()\n")
        header_file.write("}\n")
        header_file.close()

    return

def main(argv):

    log_entries_input_file = sys.argv[1]
    log_entries_header_file = sys.argv[2]

    print("Log entries input file:", log_entries_input_file)

    log_entries = []
    with open(log_entries_input_file) as input_file:
        input_file_lines = input_file.readlines()
        for line in input_file_lines:
            match = re.search(r'([A-Z_]*) (\"[A-Za-z ]*\")', line)
            log_entry = []
            if match:
                log_entry.append(match.groups()[0])
                log_entry.append(match.groups()[1])
                log_entries.append(log_entry)

    generate_header_file(log_entries, log_entries_header_file)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
