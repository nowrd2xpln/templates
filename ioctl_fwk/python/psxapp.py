#!/usr/bin/python

import fcntl
import array

# _IOWR( 'x', 0, int )


_ioc_write = 0x01	
_ioc_read = 0x02	
rw = _ioc_write | _ioc_read	#bit 31-32
size_of_args = 0x20		#bit 29-16
ascii_char =  ord('x')		#bit 15-8
function_num =  0x00		#bit 7-0

print "rw ", hex(rw)

upper_16bits = (rw << 14) | (size_of_args)
print "upper_16bits ", hex(upper_16bits)
lower_16bits = (ascii_char << 8) | (function_num)
print "lower_16bits ", hex(lower_16bits)
tmp = (upper_16bits << 16) | (lower_16bits)

print "tmp ", hex(tmp)

file = open("/dev/ioctl_dev")
buf = array.array('L', [0])
print "buf ", hex(id(buf))
print "buf ", buf
fcntl.ioctl(file, 0x5757, buf, 1)
#fcntl.ioctl(file, 0x5757, buf, 0)
