# Native-Anti-Tamper
The source of the native library can be found at /app/src/main/jni/anti_debug.cpp  
- It detects magisk(magiskhide) by checking mount paths for blacklisted strings   
- It detects frida by:  
  1. Checking the memory map of the process for frida keyword
  2. Checking the default port of the frida-server
  3. Doing portscan to check for the server running on a non-default port
- It prevents debugging by forking a child process and attaching it to the parent process using the ptrace
