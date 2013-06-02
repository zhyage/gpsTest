insmod /opt/self_module/keyboard/input-core.ko
insmod /opt/self_module/keyboard/evdev.ko
insmod /opt/self_module/keyboard/hid.ko
insmod /opt/self_module/keyboard/usbhid.ko
insmod /opt/self_module/keyboard/usbkbd.ko
mkdir -p /dev/input
mknod /dev/input/event1 c 13 65
