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
LIBS += "-lwinhttp"  # Add this line to link against WinHTTP


# C source code files that are required
CSRC  = CRYPTO_implant.c                 # The main program
CSRC += implant.pb.c             # The compiled protocol definition
CSRC += $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
CSRC += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
CSRC += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts
# CSRC += /home/ubuntu/BinaryBaddies/C2_Implant/http_client.c # Implant's HTTP functions

OBJ = "exec.o"
OBJ += "http.o"

# Build rule for the main program
implant: $(CSRC)
	$(CC) $(OBJ) $(CSRC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o cryptoimplant.exe 






