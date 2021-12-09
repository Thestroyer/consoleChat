# consoleChat
ConsoleChat is just a simple console application, that allows communication between 2 PC's. It's OS independent (was tested on lubuntu 20.04(g++), Windows 10(VS compiler) and Android 6.0(g++)), so you can compile it and run almost everywhere.
# Android
To run this application on Android, there are a few extra steps you need to do:
1. Install some terminal emulator like 'termux', I haven't tested others.
2. Find out, which cpu architecture your phone is using (like 'armv81'), just run termux and type 'uname -m'.
3. Find out, what is core bit-width of your phone's CPU, type 'cat /proc/cpuinfo' in termux.
4. Compile consoleChat with architecture of point 2 and core bit-width of point 3 (I used for this lubuntu with command 'arm-linux-gnueabi-g++').
- Warning: You probably need '-static' flag.
- Warning: If your core bit-width is 32(x86), you need to add '-mbe32' flag.
6. Copy executable from PC to your phone.
7. Copy this executable to '/data/data/com.termux/' directory ('cp' command in termux).
- Warning: If you won't do this, your program won't even run.
8. Give it execute permission - 'chmod +x [file_name]'.
9. Run program - './[file_name]'. if you've followed all the steps correctly, the program should run without errors.
