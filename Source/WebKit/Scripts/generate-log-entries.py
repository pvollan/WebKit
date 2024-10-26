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


def generate_messages_file(log_entries, log_entries_messages_file):
    print("Log entries messages file:", log_entries_messages_file)

    with open(log_entries_messages_file, 'w') as messages_file:
        messages_file.write("#if ENABLE(LOGD_BLOCKING_IN_WEBCONTENT)\n")
        messages_file.write("messages -> LogStream NotRefCounted Stream {\n")
        messages_file.write("    LogOnBehalfOfWebContent(std::span<const uint8_t> logChannel, std::span<const uint8_t> logCategory, std::span<const uint8_t> logString, uint8_t logType)\n")
        for log_entry in log_entries:
            messages_file.write("    " + log_entry[0] + "()\n")
        messages_file.write("}\n")
        messages_file.write("#endif\n")
        messages_file.close()

    return


def main(argv):

    log_entries_input_file = sys.argv[1]
    log_entries_header_file = sys.argv[2]
    log_entries_messages_file = sys.argv[3]

    print("Log entries input file:", log_entries_input_file)

    log_entries = []
    with open(log_entries_input_file) as input_file:
        input_file_lines = input_file.readlines()
        for line in input_file_lines:
            match = re.search(r'([A-Z_0-9]*) (\"[A-Za-z ]*\")', line)
            log_entry = []
            if match:
                log_entry.append(match.groups()[0])
                log_entry.append(match.groups()[1])
                log_entries.append(log_entry)

    generate_header_file(log_entries, log_entries_header_file)
    generate_messages_file(log_entries, log_entries_messages_file)

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
