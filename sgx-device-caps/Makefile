SGX_SDK ?= /opt/intel/sgxsdk


SGX_COMMON_CFLAGS := -m64
SGX_LIBRARY_PATH := $(SGX_SDK)/lib64

SGX_COMMON_CFLAGS += -O2

Libcaps_Library_Name := libsgx_capable.a

App_Cpp_Files := App.cpp
App_Include_Paths := -I$(SGX_SDK)/include
App_C_Flags := $(SGX_COMMON_CFLAGS) -fPIC -Wno-attributes $(App_Include_Paths)
App_Cpp_Flags := $(App_C_Flags) -std=c++11
App_Link_Flags := $(SGX_COMMON_CFLAGS) -L$(SGX_LIBRARY_PATH) -l:$(Libcaps_Library_Name)
App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := check-sgx-caps

.PHONY: all run

all: $(App_Name)

%.o: %.cpp
	@echo "CXX  => $(CXX)"
	@echo "App_Cpp_Flags  => $(App_Cpp_Flags)"
	$(CXX) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): $(App_Cpp_Objects)
	@echo "App_Link_Flags  => $(App_Link_Flags)"
	$(CXX) $^ -o $@ $(App_Link_Flags)
	echo "LINK =>  $@"

run: all
	@echo "RUN  =>  ./$(App_Name)"
	./$(App_Name)


.PHONY: clean
clean:
	@rm *.o $(App_Name)
