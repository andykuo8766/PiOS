cd kernel8
make
make run
dma

andy@ubuntu:~/OSC/Lab-03-v4.0/kernel8$ make run
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
VNC server running on 127.0.0.1:5900

------------------------
=        Kernel        =
------------------------
andy@pi$
andy@pi$
andy@pi$ dma

+++++++++ dma_test1 +++++++++

********* init *********
free:

pool end
used:

pool end

********* alloced *********
free:
  addr: 0x30000078         size: 0x00000F88

pool end
used:
  addr: 0x30000058         size: 0x00000008
  addr: 0x30000038         size: 0x00000008
  addr: 0x30000018         size: 0x00000008

pool end

********* freed *********
free:
  addr: 0x30000038         size: 0x00000FC8

pool end
used:
  addr: 0x30000018         size: 0x00000008

pool end

********* freed *********
free:

pool end
used:

pool end

ending
andy@pi$
