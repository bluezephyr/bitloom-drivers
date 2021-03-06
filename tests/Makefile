#-----------------------------------------------------------------------------#
#			PROJECT SPECIFICS
#-----------------------------------------------------------------------------#
# Project name and source files
NAME   := bitloom_unittests
ROOT   := ../

ifndef BITLOOM_CORE
	BITLOOM_CORE := ../../bitloom-core
endif

SRC    := test_main.c \
	  test_ssd1306.c \
	  ssd1306.c \
	  ssd1306_mock.c


# Include directories
INC    := include \
	  tests/config \
	  tests/mocks \
	  src/ssd1306 \


# Include directories (absolute paths)
INC_A  	:= $(BITLOOM_CORE)/include


#-----------------------------------------------------------------------------#
#			UNITY FRAMEWORK
#-----------------------------------------------------------------------------#
ifeq ($(UNITY_HOME),)
  $(error UNITY_HOME varaible not set in the environment)
endif

# Unity Includes
UNITY_I	:= $(UNITY_HOME)/src \
	   $(UNITY_HOME)/extras/fixture/src \
	   $(UNITY_HOME)/extras/fixture/test \
	   $(UNITY_HOME)/extras/memory/src \

# Unity source
UNITY_S := unity.c unity_fixture.c unity_memory.c

#-----------------------------------------------------------------------------#
#			MAKEFILE CORE
#-----------------------------------------------------------------------------#
.PHONY: clean all debug test
CC	= gcc

SRC_P   = $(addprefix $(ROOT), $(SRC))
INC_P   = $(addprefix $(ROOT), $(INC))
INC_L   = $(foreach d, $(INC_P), -I$(d))
INC_AL  = $(foreach d, $(INC_A), -I$(d))
INC_U	= $(foreach d, $(UNITY_I), -I$(d))
OBJ     = $(SRC:.c=.o) $(UNITY_S:.c=.o)

CFLAGS  := -Wall -g3 -I. $(INC_L) $(INC_AL) $(INC_U)
LDFLAGS :=

all: 	test

VPATH = $(UNITY_HOME)/src \
       	$(UNITY_HOME)/extras/fixture/src \
       	$(UNITY_HOME)/extras/memory/src \
	ssd1306 \
	mocks \
	../src/ssd1306 \

$(NAME) : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

#-----------------------------------------------------------------------------#
#			CLEAN
#-----------------------------------------------------------------------------#
clean:
	rm -rf $(OBJ)
	rm -f $(NAME)

#-----------------------------------------------------------------------------#
#			TEST
#-----------------------------------------------------------------------------#
test:	$(NAME)
	./$(NAME) -v

#-----------------------------------------------------------------------------#
#			DEBUG
#-----------------------------------------------------------------------------#
debug:
	@echo "ROOT    : " $(abspath $(ROOT))
	@echo "SRC_P   : " $(SRC_P)
	@echo "INC_P   : " $(INC_P)
	@echo "INC_L   : " $(INC_L)
	@echo "INC_AL  : " $(INC_AL)
	@echo "INC_U   : " $(INC_U)
	@echo "OBJ     : " $(OBJ)
	@echo "OBJ_U   : " $(OBJ_U)
	@echo "CFLAGS  : " $(CFLAGS)
	@echo "LDFLAGS : " $(LDFLAGS)
	@echo "VPATH   : " $(VPATH)
	@echo "CURDIR  : " $(CURDIR)
