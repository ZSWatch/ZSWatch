# Debugging crashes

## Coredump
Use following command to decode a coredump on a FW build locally on same computer.
By passing the `--build_dir`, we can guess both the .elf and toolchain path.
```
west coredump --coredump_file cb.txt --build_dir app/<build_folder>
```

Use when FW has been for example downloaded from GitHub, then toolchain and `.elf` file needs to be provided in order to decode the coredump.
```
west coredump --coredump_file cb.txt --elf app/<build_folder>/zephyr/zephyr.elf --toolchain /home/user/ncs/toolchains/7795df4459/opt/zephyr-sdk
```

If `--coredump_file` is omitted, then the coredump will be retreived over RTT logs.
Note this requires FW built with logs over RTT enabled. After running below, go to settings -> Other -> Dump coredump over log and rest is automatic.
```
west coredump --build_dir app/<build_folder>
```