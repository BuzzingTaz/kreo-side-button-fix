# Linux Side Button Fix for Kreo Chimera

The mouse describes itself as a 3 button mouse in the report descriptor. This module intercepts this comunication and changes the descriptor to appear as a 5 button device.
The values of `KREO_VID` and `KREO_PID` in `kreo_side_fixup.c` might be different. Check using `lsusb`.

## Deps
- clang

## Usage

Compile
```
$ make
```

Load module
```
# insmod kreo_side_fixup.ko
```

Unload module
```
# rmmod kreo_side_fixup
```

## Load on boot

Install module
```
# mkdir -p /lib/modules/$(uname -r)/extra

# cp kreo-fixup.ko /lib/modules/$(uname -r)/extra/

# depmod -a
```

Load on boot
```
echo "kreo_side_fixup" | sudo tee /etc/modules-load.d/kreo_side_fixup.conf
```

## On kernel update
```
make clean

make

cp kreo_side_fixup.ko /lib/modules/$(uname -r)/extra/

depmod -a
```
