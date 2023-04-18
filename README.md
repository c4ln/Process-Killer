# Process-Killer
Kill a process by utilizing a vulnerable kernel driver. 

IM USING hacksysextremevulnerabledriver [HEVD] for this demo!
 
This program that loads a driver and then monitors a process, which is hardcoded. Can be changed at line 93. It opens a handle to the device and sends an IOCTL command to it. The program then checks all running processes to see if the process is running. If it is found, the program saves its PID and sends the PID to the driver through the same IOCTL command. This causes the driver to terminate the process with the given PID. The program can be compiled and run on a Windows machine with a vulnerable driver installed.

Note: The program requires the driver to be unsigned and the system to have test signing enabled to load the driver. Disabling driver signature verification should only be done in a secure, isolated environment for testing purposes.


If you get the following errors,

1) Windows cannot verify the digital signature for this file. A recent hardware or software change might have installed a file that is signed incorrectly or damaged, or that might be malicious software from an unknown source.

Execute cmd/pwsh as an admin, and execute bcdedit /set testsigning on
restart your computer.

2) After doing that you face 
An error has occurred setting the element data.
The value is protected by Secure Boot policy and cannot be modified or deleted.

 you will need to temporarily disable Secure Boot. The steps to do this will vary depending on your system, but you can typically do it by accessing the BIOS or UEFI settings during startup and disabling Secure Boot from there.

Once Secure Boot is disabled, you can run the bcdedit /set testsigning on and it should work
