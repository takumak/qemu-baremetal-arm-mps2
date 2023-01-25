# QEMU arm baremetal app example

## build

```bash
$ sudo apt-get install gcc-arm-none-eabi # ubuntu
$ make
```

## run

```bash
$ qemu-system-arm -cpu cortex-m4 -M mps2-an386 -semihosting -kernel image.bin -monitor null -serial stdio -nographic
Hello, world
circle_area(3) = 28.274334
```
