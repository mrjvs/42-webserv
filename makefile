# Constants
NAME			= not-apache
NAME			:= $(addprefix ./build/,$(NAME))

# Compiler
CXX				= clang++

CXX_FLAGS		= -Wall -Werror -Wextra -std=c++98 -pedantic-errors
DEBUG_FLAGS		=
BUILD_FLAGS		= -O3 -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ifeq ($(CXX),g++)
		CXX_FLAGS += -fPIE
	endif
endif

ifdef BUILD_DEBUG
DEBUG_FLAGS		= -g -fsanitize=address
BUILD_FLAGS		= -pthread
endif
ifdef BUILD_LEAK
DEBUG_FLAGS		= -g -fsanitize=leak
BUILD_FLAGS		= -pthread
endif

# Files
SRC_DIR			= src
BUILD_DIR		= build
OUT_DIR			= build/out
INC_DIR			= src/includes

SRC	=\
	main.cpp\
	log/Loggable.cpp\
	log/Logger.cpp\
	log/LogItem.cpp\
	server/handlers/AHandler.cpp\
	server/handlers/StandardHandler.cpp\
	server/listeners/TCPListener.cpp\
	server/listeners/TerminalListener.cpp\
	server/parsers/AParser.cpp\
	server/parsers/Request.cpp\
	server/parsers/HTTPParser.cpp\
	server/parsers/ft_utils.cpp\
	server/parsers/TerminalParser.cpp\
	server/responders/AResponder.cpp\
	server/responders/HTTPResponder.cpp\
	server/responders/TerminalResponder.cpp\
	server/Client.cpp\
	server/Server.cpp\
	server/TerminalClient.cpp

HEADERS	=\
	log/Loggable.hpp\
	log/Logger.hpp\
	log/LogItem.hpp\
	server/handlers/AHandler.hpp\
	server/handlers/StandardHandler.hpp\
	server/listeners/AListener.hpp\
	server/listeners/TCPListener.hpp\
	server/listeners/TerminalListener.hpp\
	server/parsers/AParser.hpp\
	server/parsers/Request.hpp\
	server/parsers/HTTPParser.hpp\
	server/parsers/ft_utils.hpp\
	server/parsers/TerminalParser.hpp\
	server/responders/AResponder.hpp\
	server/responders/HTTPResponder.hpp\
	server/responders/TerminalResponder.hpp\
	server/Client.hpp\
	server/Server.hpp\
	server/ServerTypes.hpp\
	server/TerminalClient.hpp

# Fix sources and headers
OBJ				= $(patsubst %.cpp,%.o,$(SRC))
HEADERS			:= $(addprefix $(INC_DIR)/,$(HEADERS))

# Colours
DARK_GREEN		= \033[0;32m
GREEN			= \033[0;92m
END				= \033[0;0m

PREFIX			= $(DARK_GREEN)$(notdir $(NAME)) $(END)>>

# Rules
.PHONY: all clean fclean re exec valgrind debug rdebug leak rleak

all: $(NAME)

deps:
	@echo "$(SRC)"

$(NAME): $(addprefix $(OUT_DIR)/,$(OBJ))
	@echo "$(PREFIX)$(GREEN) Bundling objects...$(END)"
	@echo "BUILD $(NAME) $(CXX_FLAGS) $(DEBUG_FLAGS) $(BUILD_FLAGS)"
	@$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) $(BUILD_FLAGS) -I$(INC_DIR) -o $@ $(addprefix $(OUT_DIR)/,$(OBJ))

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "$(PREFIX)$(GREEN) Compiling file $(END)$(notdir $<) $(GREEN)to $(END)$(notdir $@)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) -I$(INC_DIR) -o $@ -c $<

clean:
	@echo "$(PREFIX)$(GREEN) Removing directory $(END)$(OUT_DIR)"
	@rm -rf $(OUT_DIR)

fclean: clean
	@echo "$(PREFIX)$(GREEN) Removing file $(END)$(NAME)"
	@rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

exec:
	$(MAKE) all
	./$(NAME)

valgrind:
	valgrind --undef-value-errors=no --leak-check=full ./$(NAME)

debug:
	$(MAKE) BUILD_DEBUG=1 all

rdebug:
	$(MAKE) fclean
	$(MAKE) debug

leak:
	$(MAKE) BUILD_LEAK=1 all

rleak:
	$(MAKE) fclean
	$(MAKE) leak