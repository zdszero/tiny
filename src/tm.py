#! /usr/bin/python3

import re
import sys

DEBUG = False

#################################
#  SOME LANGUAGE RELATED STUFF  #
#################################

REG_COUNT = 8
PC_REG = 7

IADDR_SIZE = 1024
DADDR_SIZE = 1024

OpKind = (
#     0       1     2      3      4      5      6
    'HALT', 'IN', 'OUT', 'ADD', 'SUB', 'MUL', 'DIV', # RO
#    7     8 
    'LD', 'ST',                                      # RM
#     9    10      11     12     13     14     15
    'LDA','LDC' ,'JLT', 'JGE' , 'JGT', 'JEQ', 'JNE'  # RA
)

StepResult = ('OKAY', 'HALT', 'IMEM_ERR', 'DMEM_ERR', 'DIVZERO')

StepResultTab = ['OK', 'Halted', 'Instruction Memory Fault', 
    'Data Memory Fault','Divide By Zero Falut']

class Instruction:
    def __init__(self, op: int, arg1: int, arg2: int, arg3: int):
        self.op   = op
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3

#######################################
#  SOME VARIABLES USED FOR EXECUTION  #
#######################################

regex = re.compile(r'^\s*(\d+):\s+(\w+)\s+(\d),(-?\d+),?\(?(\d)\)?.*$')

regs = [ 0 for i in range(REG_COUNT) ]

iMem = [ None for i in range(IADDR_SIZE) ]
dMem = [ 0 for i in range(DADDR_SIZE) ]
dMem[0] = DADDR_SIZE - 1

iloc = 0
dloc = 0

traceflag = False

if DEBUG:
    traceflag = True


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
                print('{}: {} {} {} {}'.format(mo.group(1), mo.group(2), mo.group(3), mo.group(4), mo.group(5)))
            location =  int(mo.group(1))
            opcode = OpKind.index(mo.group(2))
            arg1 = int(mo.group(3))
            arg2 = int(mo.group(4))
            arg3 = int(mo.group(5))
            iMem[location] = Instruction(opcode, arg1, arg2, arg3)

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
        print('{}:  {:<4} {},{},{}'.format(loc, opcode_str, arg1, arg2, arg3))
    # RM and RA
    elif 7 <= opcode <= 15:
        print('{}:  {:<4} {},{}({})'.format(loc, opcode_str, arg1, arg2, arg3))
    else:
        print('opcode overflow !!!')
        sys.exit(2)

def single_step():
    pc = regs[PC_REG]
    if pc < 0 or pc >= IADDR_SIZE:
        return StepResult.index('IMEM_ERR')
    ins = iMem[pc]
    regs[PC_REG] = pc + 1
    r, s, t, a = 0, 0, 0, 0
    opcode = ins.op
    # opclass
    if 0 <= opcode <= 6:
        r = ins.arg1
        s = ins.arg2
        t = ins.arg3
    elif 7 <= opcode <= 8:
        r = ins.arg1
        s = ins.arg2
        a = regs[ins.arg3] + ins.arg2
        if a < 0 or a >= DADDR_SIZE:
            if DEBUG:
                print('regs[ins.arg3]: {}'.format(regs[ins.arg3]))
                print('r: {}  s: {}  a: {}'.format(r, s, a))
            return StepResult.index('DMEM_ERR')
    elif 9 <= opcode <= 15:
        r = ins.arg1
        s = ins.arg2
        a = regs[ins.arg3] + ins.arg2
    else:
        print('opcode overflow !!!')
        sys.exit(2)
    # concrete operation
    if opcode == 0:    # halt
        print('HALT {},{},{}'.format(r, s, t))
        return StepResult.index('HALT')
    elif opcode == 1:  # in
        flag = True
        while flag == True:
            try:
                val = int(input('Enter value for IN instruction: '))
                flag = False
                regs[r] = val
            except:
                print('Invalid Input')
    elif opcode == 2:  # out
        print('OUT instruction prints: {}'.format(regs[r]))
    elif opcode == 3:  # add
        regs[r] = regs[s] + regs[t]
    elif opcode == 4:  # sub
        regs[r] = regs[s] - regs[t]
    elif opcode == 5:  # mul
        regs[r] = regs[s] * regs[t]
    elif opcode == 6:  # div
        if regs[t] == 0:
            return StepResult.index('DIVZERO')
        regs[r] = regs[s] / regs[t]
    elif opcode == 7:  # ld
        regs[r] = dMem[a]
    elif opcode == 8:  # st
        dMem[a] = regs[r]
    elif opcode == 9:  # lda
        if DEBUG:
            print('LDA -> r: {}  a: {}'.format(r, a))
        regs[r] = a
    elif opcode == 10:  # ldc
        regs[r] = s
    elif opcode == 11: # jlt
        if regs[r] < 0: 
            regs[PC_REG] = a
    elif opcode == 12:  # jge
        if regs[r] == 0:
            regs[PC_REG] = a
    elif opcode == 13:  # jgt
        if regs[r] > 0:
            regs[PC_REG] = a
    elif opcode == 14:  # jeq
        if regs[r] == 0:
            regs[PC_REG] = a
    elif opcode == 15:  # jne
        if regs[r] != 0:
            regs[PC_REG] = a
    else:
        print('opcode overflow !!!')
        sys.exit(2)
    return StepResult.index('OKAY')

def print_regs():
    for i in range(REG_COUNT):
        print('{}: {:>5}    '.format(i, regs[i]), end='')
        if (i + 1) % 4 == 0:
            print()


def do_command() -> bool:
    global traceflag
    longcmd = input('Enter Command: ')
    cmds = re.split(r'\s+', longcmd)
    cmd = longcmd[0]
    cmd_count = len(cmds)
    step_count = 0
    print_count = 1

    if cmd == 'h':
        print('Commands are:')
        print('   s(tep <n>      '\
             'Execute n (default 1) TM instructions')
        print('   g(o            '\
             'Execute TM instructions until HALT')
        print('   r(egs          '\
             'Print the contents of the registers')
        print('   i(Mem <s <e>>  '\
             'Print n iMem locations starting at b')
        print('   d(Mem <s <e>>  '\
             'Print n dMem locations starting at b')
        print('   t(race        '\
             'Toggle instruction trace')
        print('   h(elp          '\
             'Cause this list of commands to be printed')
        print('   q(uit          '\
             'Terminate the simulation\n')
    elif cmd == 's':
        if cmd_count == 1:
            step_count = 1
        elif cmd_count == 2:
            step_count = int(cmd[1])
        else:
            print('Wrong Command Format')
    elif cmd == 'g':
        step_count = 1
    elif cmd == 'r':
        print_regs()
    elif cmd == 'i':
        try:  # avoid wrong int transformation
            if cmd_count == 2:
                iloc = int(cmds[1])
                if 0 <= iloc < IADDR_SIZE:
                    write_instruction(iloc)
                else:
                    print('Wrong Address')
            elif cmd_count == 3:
                iloc = int(cmds[1])
                print_count = int(cmds[2])
                if iloc >= 0 and iloc + print_count < IADDR_SIZE:
                    for i in range(iloc, iloc + print_count):
                        write_instruction(i)
                else:
                    print('Wrong Address')
            else:
                print('Wrong Command Format')
        except:
            print('Wrong Address Format')
    elif cmd == 'd':
        try:
            if cmd_count == 2:
                dloc = int(cmds[1])
                if 0 <= dloc < DADDR_SIZE:
                    print('{}: {}'.format(dloc, dMem[dloc]))
                else:
                    print('Wrong Address')
            elif cmd_count == 3:
                dloc = int(cmds[1])
                print_count = int(cmds[2])
                if dloc >= 0 and dloc + print_count < DADDR_SIZE:
                    for i in range(dloc, dloc + print_count):
                        print('location {}: {}'.format(i, dMem[i]))
                else:
                    print('Wrong Address')
        except:
            print('Wrong Command Format')
    elif cmd == 't':
        traceflag = not traceflag
        if traceflag == True:
            print('instruction trace on')
        else:
            print('instruction trace off')
    elif cmd == 'q':
        return False
    else:
        print('Wrong Command Format')

    step_result = StepResult.index('OKAY')
    if step_count > 0:
        if cmd == 'g':
            step_count = 0
            while step_result == StepResult.index('OKAY'):
                iloc = regs[PC_REG]
                if traceflag:
                    write_instruction(iloc)
                step_result = single_step()
                if DEBUG:
                    print_regs()
                step_count += 1
            if DEBUG:
                print('Step Result: {}'.format(StepResultTab[step_result]))
            return False
        else:
            while step_count > 0 and step_result == StepResult.index('OKAY'):
                iloc = regs[PC_REG]
                if traceflag:
                    write_instruction(iloc)
                step_result = single_step()
                step_count -= 1
            print('Step Result: {}'.format(StepResultTab[step_result]))
    return True
 


if __name__ == '__main__':
    if not len(sys.argv) == 2:
        print('usgae: {} <filename>'.format(sys.argv[0]))
        sys.exit(1)
    filename = sys.argv[1]
    read_instructions(filename)
    print('TM simulation (enter h for help) starts ...')
    done = False
    while done == False:
        done = not do_command()
    print('Simulation Done')
