NAME=not-apache
INCLUDES=./src/includes
FLAGS=-Wall -Wextra -Werror -std=c++98 -pedantic-errors
DEBUG= -g -fsanitize=address
CC=clang++

HEADERS=ConfigParser.hpp \
		ft_utils.hpp \
		getLine.hpp \
		Location.hpp \
		Server.hpp \

FILES=\
	main.cpp \
	ConfigParser.cpp \
	ft_utils.cpp \
	getLine.cpp \
	Location.cpp \
	Server.cpp

FILES:=$(addprefix ./src/, $(FILES))
HEADERS:=$(addprefix ./src/includes/, $(HEADERS))
OBJS=$(FILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	$(CC) -I $(INCLUDES) $(FLAGS) $(DEBUG) -o $(NAME) $(OBJS)
	rm -f $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) -I $(INCLUDES) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
