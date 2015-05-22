#!/usr/bin/perl 

printf("Hello Allen\n");

require "sys/ioctl.ph";

#open($FH, "+</dev/ioctl_dev") or die "Could not open devce file : $!\n";
#open($FH, "/dev/ioctl_dev") or die "Could not open devce file : $!\n";
sysopen($FH, "/dev/ioctl_dev", O_RDWR);
$buffer = 0x1234567;
$buffAddr = \$buffer;

printf("0x%p - 0x%x\n", $buffAddr, $buffer);

$ret = ioctl($FH, 0x5757, 0x01);

printf("ret 0x%x\n", $ret);

if (ret) {
        #@ary = unpack($sgttyb_t,$sgttyb);
        #$ary[2] = 127;
        #$sgttyb = pack($sgttyb_t,@ary);
}
else
{
  close($FH);
  die "Could not send ioctl!\n";
}

close($FH);
printf("Goodbye Allen\n");
