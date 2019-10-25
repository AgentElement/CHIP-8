#! /usr/bin/env python3
import re
import argparse
import sys


class Assembler:
    def __init__(self):
        pass

    @staticmethod
    def assemble(infile, outfile):
        with open(infile, 'r') as infile, open(outfile, 'wb') as outfile:
            line = infile.readline()
            while line:
                opcode = re.sub(r'#.*', '', line).strip()
                outfile.write(bytes.fromhex(opcode))
                line = infile.readline()

    @staticmethod
    def disassemble(infile, outfile):
        with open(infile, 'rb') as infile, open(outfile, 'w') as outfile:
            opcode = infile.read(2)
            while opcode:
                outfile.write("".join("%02x" % b for b in opcode) + '\n')
                opcode = infile.read(2)


def parse():
    parser = argparse.ArgumentParser()

    parser.add_argument('-o',
                        help='Output file. Defaults to rom.out',
                        type=str,
                        default='rom.out',
                        dest='output')

    parser.add_argument('-i',
                        help='Input file. Required argument.',
                        type=str,
                        required=True,
                        dest='input')

    parser.add_argument('-d', '--disassemble',
                        help='Disassembles input file from CHIP-8 bytecode to readable hex',
                        action='store_true')

    return parser.parse_args(sys.argv[1:])


def main():
    args = parse()

    asm = Assembler()

    try:
        if args.disassemble:
            asm.disassemble(args.input, args.output)
        else:
            asm.assemble(args.input, args.output)
    except FileNotFoundError:
        print('Input File not found.')


if __name__ == '__main__':
    main()
