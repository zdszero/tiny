from enum import Enum
import re
import sys

DEBUG = True

#################################
#  SOME LANGUAGE RELATED STUFF  #
#################################

REG_COUNT = 8
PC_REG = 7

IADDR_SIZE = 1024
DADDR_SIZE = 256

OpClass = ('RO', 'RM', 'RA')

OpKind = (
    'HALT', 'IN', 'OUT', 'ADD', 'SUB', 'MUL', 'DIV', # RO
    'LD', 'LDA', 'LDC', 'ST',                        # RM
    'JLT', 'JGE' , 'JGT', 'JEQ', 'JNE'               # RA
)

StepResult = {'OKAY', 'HATL', 'IMEM_ERR', 'DMEM_ERR', 'DIVZERO'}

class Instruction:
    def __init__(self, op: int, arg1: int, arg2: int, arg3: int):
        self.op   = op
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3

#######################################
#  SOME VARIABLES USED FOR EXECUTION  #
#######################################

regex = re.compile(r'^\s*\d+:\s+(\w+)\s+(\d),(-?\d+),?\(?(\d)\)?.*$')

regs = [ 0 for i in range(REG_COUNT) ]

iMem = []
dMem = [ 0 for i in range(DADDR_SIZE) ]
dMem[0] = DADDR_SIZE - 1

iloc = 0
dloc = 0

#######################################
#  SOME FUNCTIONS USED FOR EXUCUTION  #
#######################################

def read_instructions(filename: str):
    with open(filename) as fp:
        for line in fp.readlines():
            # annotation
            if line.startswith('*'):
                continue
            mo = regex.match(line)
            if DEBUG and mo == None:
                print(line)
            assert(mo != None)
            if DEBUG:
                print('{} {} {} {}'.format(mo.group(1), mo.group(2), mo.group(3), mo.group(4)))
            opcode = OpKind.index(mo.group(1))
            arg1 = int(mo.group(2))
            arg2 = int(mo.group(3))
            arg3 = int(mo.group(4))
            iMem.append(Instruction(opcode, arg1, arg2, arg3))

def write_instruction(loc: int):
    if loc > len(iMem):
        print('location to large')
        return
    ins = iMem[loc]
    opcode = ins.op
    opcode_str = OpKind[opcode]
    arg1 = ins.arg1
    arg2 = ins.arg2
    arg3 = ins.arg3
    # RO
    if 0 <= opcode <= 6:
        print('{:<4} {},{},{}'.format(opcode_str, arg1, arg2, arg3))
    # RM and RA
    elif 7 <= opcode <= 15:
        print('{:<4} {},{}({})'.format(opcode_str, arg1, arg2, arg3))
    else:
        print('opcode overflow !!!')
        sys.exit(2)

def single_step():
    pc = iMem[PC_REG]
    ins = iMem[pc]
    iMem[PC_REG] = pc + 1
    r, s, t, a = 0, 0, 0, 0
    if 0 <= ins.op <= 6:
        r = ins.arg1
        s = ins.arg2
        t = ins.arg3
    elif 7 <= ins.op <= 10:
        r = ins.arg1
        s = ins.arg2
        a = regs[ins.arg3] + ins.arg2
    elif 11 <= ins.op <= 15:
        pass



if __name__ == '__main__':
    if not len(sys.argv) == 2:
        print('usgae: {} <filename>'.format(sys.argv[0]))
        sys.exit(1)
    filename = sys.argv[1]
