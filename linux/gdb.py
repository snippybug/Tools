import gdb
import re
import random

gdb.execute("set pagination off")

mem   = dict()
insts = list()

n32_to_o32 = {
    # Same
    'zero'  : 'zero',
    'at'    : 'at',
    'v0'    : 'v0',
    'v1'    : 'v1',
    'a0'    : 'a0',
    'a1'    : 'a1',
    'a2'    : 'a2',
    'a3'    : 'a3',
    's0'    : 's0',
    's1'    : 's1',
    's2'    : 's2',
    's3'    : 's3',
    's4'    : 's4',
    's5'    : 's5',
    's6'    : 's6',
    's7'    : 's7',
    'k0'    : 'k0',
    'k1'    : 'k1',
    'gp'    : 'gp',
    'sp'    : 'sp',
    's8'    : 's8',
    'fp'    : 'sp',
    'ra'    : 'ra',
    't8'    : 't8',
    't9'    : 't9',
    # Different
    'a4' : 't0',
    'a5' : 't1',
    'a6' : 't2',
    'a7' : 't3',
    't0' : 't4',
    't1' : 't5',
    't2' : 't6',
    't3' : 't7',
}

def convert_o32(regs):
    return set(n32_to_o32[reg] for reg in regs)

# zero, at, k0, k1, gp, sp, s8(fp), ra
opt_regs = set(['v0','v1','a0','a1','a2','a3','a4','a5','a6','a7','t0','t1','t2','t3','t8','t9','s0','s1','s2','s3','s4','s5','s6','s7'])
# o32
opt_regs = convert_o32(opt_regs)
written_regs = set()
init_regs = set()
avail_regs = opt_regs
inst_addrs = list()

def is_branch(func):
    return (func == 'bnez' or func == 'b')

def is_reg_op(op):
    return op in n32_to_o32.keys()

def dump_bytes(array):
    for b in array:
        print("\t.byte 0x%x" % b)

def write_log(s):
    gdb.write(s, gdb.STDLOG)

def insert_val(datas, idx, val, t):
    size = len(datas)
    if idx + 7 >= size:
        for j in range(size, idx + 8):
            datas.append(0)
    if t == 1:
        for i in range(8):
            datas[idx + i] = val & 0xff
            val = val >> 8

def dump_mem():
    for (addr, val, t) in mem.items():
        print("0x%x: 0x%x, %d" % (addr, val, t))

def reg_value(reg):
    val = int(gdb.execute("info registers " + reg, to_string = True).split()[1], base = 16)
    #print("%s,Get: 0x%x" % (reg,val))
    return val

def exe_inst():
    gdb.execute("stepi")

def handle_regs(dst, srcs):
    for src in srcs:
        avail_regs.discard(src)
        if src not in written_regs:
            init_regs.add((src, reg_value(src)))
    written_regs.add(dst)
    avail_regs.discard(dst)

def handle_load(ops, idx):
    dst = ops[0]
    op1 = re.split('[()]', ops[1])
    off = op1[0]
    src = op1[1]
    handle_regs(dst,[])
    addr = reg_value(src) + int(off)
    inst_addrs.append((idx, addr))
    exe_inst()
    val = reg_value(dst)
    mem[addr] = (val, 1)
    print("%d: 0x%x" % (idx, addr))

def handle_store(ops, idx):
    dst = ops[0]
    op1 = re.split('[()]', ops[1])
    off = op1[0]
    src = op1[1]
    handle_regs(dst, [])
    addr = reg_value(src) + int(off)
    inst_addrs.append((idx, addr))
    mem[addr] = (0, 0)
    exe_inst()
    print("%d: 0x%x" % (idx, addr))

def skip_2op(ops, idx):
    dst = ops[0]
    src0 = ops[1]
    src1 = ops[2]
    handle_regs(dst, [src0,src1])
    exe_inst()

def skip_3op(ops, idx):
    dst = ops[0]
    src0 = ops[1]
    src1 = ops[2]
    src2 = ops[3]
    handle_regs(dst, [src0,src1,src2])
    exe_inst()

def skip_1op(ops, idx):
    dst = ops[0]
    src0 = ops[1]
    handle_regs(dst, [src0])
    exe_inst()

def handle_mtc1(ops, idx):
    src = ops[0]
    dst = ops[1]
    handle_regs(dst, [src])
    exe_inst()

handlers = {
    'lw'        : handle_load,
    'lwc1'      : handle_load,
    'ld'        : handle_load,
    'ldc1'      : handle_load,
    'lsa'       : skip_2op,
    'addiu'     : skip_1op,
    'dsll'      : skip_1op,
    'daddu'     : skip_2op,
    'sub.s'     : skip_2op,
    'sub.d'     : skip_2op,
    'swc1'      : handle_store,
    'sdc1'      : handle_store,
    'mul.s'     : skip_2op,
    'mfc1'      : skip_1op,
    'dmfc1'     : skip_1op,
    'madd.s'    : skip_3op,
    'madd.d'    : skip_3op,
    'dlsa'      : skip_2op,
    'rsqrt.s'   : skip_1op,
    'mov.s'     : skip_1op,
    'mtc1'      : handle_mtc1,
    'dmtc1'     : handle_mtc1,
    'daddiu'    : skip_1op,
    'msub.s'    : skip_3op,
    'add.s'     : skip_2op,
    'nmsub.s'   : skip_3op,
    'nmsub.d'   : skip_3op,
    'add.d'     : skip_2op,
    'andi'      : skip_1op,
    'recip.d'   : skip_1op,
    'mul.d'     : skip_2op,
    'neg.d'     : skip_1op,
}

gdb.execute("set args 3000 reference.dat 0 0 100_100_130_ldc.of")
bp0 = gdb.Breakpoint("*0x120004318")
#bp1 = gdb.Breakpoint("*0x12000d2b4")
bp1 = gdb.Breakpoint("*0x120004300")

stop_pc = bp1.location[1:]

index = 0
gdb.execute("run")
while True:
    s = gdb.execute("x /i $pc", to_string = True)
    # Example: => 0x12000d268 <inl1130_+1736>:  swc1    $f3,68(sp)
    inst = re.split('\s+', s, 3)[3];
    print(inst)
    fields = inst.split()
    func = fields[0]
    if is_branch(func):
        s = gdb.execute("x /i $pc + 4", to_string = True)
        inst = re.split('\s+', s, 3)[3]
        fields = inst.split()
        func = fields[0]
    insts.append("\t" + inst)
    #print("inst: " + inst)
    ops = fields[1].split(',')
    ops = [n32_to_o32[op] if is_reg_op(op) else op for op in ops]
    # Execute the instruction
    handlers[func](ops, index)
    index = index + 1

    pc = str(gdb.parse_and_eval("$pc")).split()[0]
    if  pc == stop_pc:
        break

log_file = "episode.S"
gdb.execute("set logging file %s" % log_file)
gdb.execute("set logging overwrite on")
gdb.execute("set logging on")
write_log("# Notice the ABI is o32.\n")
write_log("\t.data\n")

addrs = mem.keys()
label_tpl = "data%d"
name = ""
l_count = 0
start_addr = 0
cur_addr = 0
bytes_array = list()
addr_labels = dict()

init_code = ""
sel_reg = ""
use_vreg = False
vreg_cnt = 0

if len(avail_regs) < len(addrs):
    use_vreg = True

# Relocate data
addrs.sort()
for addr in addrs:
    idx = addr - start_addr
    if addr - cur_addr > 8:
        if bytes_array:
            dump_bytes(bytes_array)
        idx = 0
        start_addr = addr
        name = label_tpl % l_count
        if use_vreg:
            sel_reg = "p%d" % vreg_cnt
            vreg_cnt = vreg_cnt + 1
        else:
            sel_reg = avail_regs.pop()
        init_code = init_code + "\tla\t%s,%s\n" % (sel_reg, name)
        print(name + ": ")
        print("\t.align 6")
        l_count = l_count + 1
        bytes_array = []

    addr_labels[addr] = (sel_reg, start_addr)
    cur_addr = addr
    (val, t) = mem[addr]
    #print("0x%x: 0x%x" % (addr, val))
    insert_val(bytes_array, idx, val, t)

if bytes_array:
    dump_bytes(bytes_array)

write_log("\n")
write_log("\t.text\n")

# Address init
write_log("# Address init\n")
if use_vreg:
    write_log("# " + str(avail_regs))
write_log(init_code)
# Register init
write_log("# Register init\n")
for (reg, val) in init_regs:
    write_log("\tli\t%s,0x%x\n" % (reg, val))

write_log("# Code body\n")
for (ic, addr) in inst_addrs:
    inst = insts[ic]
    (reg, base) = addr_labels[addr]
    off = addr - base
    fields = inst.split()
    op_addr = fields[1].split(',')
    op_addr[1] = "%d(%s)" % (off, reg)
    insts[ic] = "\t%s\t%s,%s\n" % (fields[0], op_addr[0], op_addr[1])

for inst in insts:
    write_log(inst)

gdb.execute("set logging off")
gdb.execute("set confirm off")
gdb.execute("quit")
