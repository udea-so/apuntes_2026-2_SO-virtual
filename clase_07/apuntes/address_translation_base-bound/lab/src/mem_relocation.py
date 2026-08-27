import sys
import random

def print_header():
  title = '\nBase and Boards Information'
  dashes = '-' * len(title)
  base = 'Base: 0x0003082 (decimal: 12418)'
  limit = 'Limit: 472\n'
  va_trace = 'Virtual Address Trace:'

  print(title)
  print(dashes)
  print(base)
  print(limit)
  print(va_trace)

def make_hex(n):
  hex_num = hex(n)
  num_str = str(hex_num)
  index = num_str.find('x') + 1
  final_string = num_str[:index] + '0' + num_str[index:]
  return final_string

def print_trace(n):
  if n == 1:
    print(f'  * {make_hex(425)} (decimal: 425)')
    print(f'  * {make_hex(257)} (decimal: 257)')
    print(f'  * {make_hex(541)} (decimal: 541)')
    print(f'  * {make_hex(386)} (decimal: 386)')
    print(f'  * {make_hex(817)} (decimal: 817)')
    print()
  elif n == 2:
    print(f'  * {make_hex(483)} (decimal: 483)')
    print(f'  * {make_hex(369)} (decimal: 369)')
    print(f'  * {make_hex(648)} (decimal: 648)')
    print(f'  * {make_hex(429)} (decimal: 429)')
    print(f'  * {make_hex(211)} (decimal: 211)')
    print()
  elif n == 3:
    print(f'  * {make_hex(472)} (decimal: 472)')
    print(f'  * {make_hex(182)} (decimal: 182)')
    print(f'  * {make_hex(566)} (decimal: 566)')
    print(f'  * {make_hex(469)} (decimal: 469)')
    print(f'  * {make_hex(473)} (decimal: 473)')
    print()

sim = int(sys.argv[1])

print_header()
print_trace(sim)
