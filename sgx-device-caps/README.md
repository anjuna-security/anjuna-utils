------------------------
check-sgx-caps
------------------------
The project uses the Intel SGX SDK to compile a utility that checks if the
host is capable of running SGX applications.

------------------------------------
How to Build/Execute the utility
------------------------------------
1. Install Intel(R) SGX SDK for Linux* OS
2. Make sure your environment is set:
    $ source ${sgx-sdk-install-path}/environment
3. Build the project with the prepared Makefile:
        $ make
4. Execute the binary directly:
    $ ./check-sgx-caps
5. Execute the binary using make
    $ make run
6. Clean the project
    $ make clean
