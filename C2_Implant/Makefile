# Include the nanopb provided Makefile rules

# include ../../extra/nanopb.mk
NANOPB_DIR := /home/ubuntu/BinaryBaddies/nanopb/

CC=x86_64-w64-mingw32-gcc

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0
CFLAGS += "-I$(NANOPB_DIR)"
CFLAGS += "-I./include" 
CFLAGS += "-DPB_ENABLE_MALLOC=1"

# Linker flags to specify the library directory and libraries
LDFLAGS += "-L/usr/x86_64-w64-mingw32/lib"
LIBS += -Wl,-Bstatic -lsodium -Wl,-Bdynamic
LIBS += "-lwinhttp"

# C source code files that are required
CSRC  = implant.c                   # The main program
CSRC += implant.pb.c                # The compiled protocol definition
CSRC += $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
CSRC += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
CSRC += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts
# CSRC += http_client.c		   # Implant's HTTP functions
CSRC += list_directory.c
CSRC += change_directory.c


OBJ = "obj/exec.o"
OBJ += "obj/http.o"

# Build rule for the main program
implant: $(CSRC)
	$(CC)  $(OBJ) $(CSRC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o implant.exe  

