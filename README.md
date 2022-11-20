# kernel_user_communication
It is the communicator between kernel and user mode. It is uses DO_BUFFERED_IO and can be used with DeviceIoControl (with IOCTL codes) or ReadFile/WriteFile functions.

![image](https://user-images.githubusercontent.com/57231832/202889778-6e9ae3db-8bcc-4e6a-a61c-4f13bfee3bac.png)

# References
1. https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/using-buffered-i-o
