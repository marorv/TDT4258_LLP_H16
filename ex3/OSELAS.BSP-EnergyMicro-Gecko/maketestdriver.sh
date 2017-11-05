ptxdist clean test-driver-gamepad
ptxdist compile test-driver-gamepad
ptxdist targetinstall test-driver-gamepad
ptxdist targetinstall kernel
ptxdist image root.romfs
ptxdist test flash-rootfs
