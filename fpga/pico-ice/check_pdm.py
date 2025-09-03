
# Address map
# https://github.com/TinyTapeout/ttsky25a-tinyQV/blob/main/docs/info.md#address-map

import machine
import sys
import time

# make sure we are on tinyQV, not the RP2040 on pico-ice
assert 'tinyQV' in sys.implementation._machine

print('start')

PERIPHERAL=10
base = 0x800_0000 + (64*PERIPHERAL)
print('base', hex(base))

def modify_register(reg, data):
    before = machine.mem32[base + reg]
    machine.mem32[base + reg] = data
    after = machine.mem32[base + reg]
    if after != data:
        print('FAILED')
        #raise Exception('failed register modification')
        # XXX: exceptions seem to hang MicroPython?
    else:
        print('MOD', reg, data, before, after)

# Enable our peripheral to control output pins
out6 = machine.Pin(6, func_sel=PERIPHERAL)
#FUNC_SEL_BASE = 0x8000060
#for pin in range(0, 2):
#    machine.mem32[FUNC_SEL_BASE + pin] = PERIPHERAL

    

REG_CTRL = 0x00
REG_CLKP = 0x04
REG_PCMW = 0x08

# Try enable PDM clock
print('Enable PDM clock')
modify_register(REG_CLKP, 14)
modify_register(REG_CTRL, 1)
print('PDM clock enabled')

while True:
    time.sleep(2.0)
    modify_register(REG_CTRL, 0)
    time.sleep(2.0)
    modify_register(REG_CTRL, 1)


print('end')
