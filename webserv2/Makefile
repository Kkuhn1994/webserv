NAME		=	webserv

CC				=	c++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++11 -I $(HEADER_DIR)

HEADER_DIR		=	./include/
OBJ_DIR		=	./obj/
SRC_DIR		=	./src/
SOURCES		=	$(addprefix $(SRC_DIR),$(addsuffix .cpp,$(FILES)))
OBJECTS		=	$(addprefix $(OBJ_DIR),$(addsuffix .o,$(FILES)))

FILES	=	\
			ConfService \
			ServerBlock \
			LocationRedirect \
			Request \
			utility \
			Server \
			CGIExecutor \
			main


$(NAME): $(OBJECTS)
	@$(CC) $(CXXFLAGS) $(SOURCES) -o $@

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@echo "🧹 Cleaning object files..."
	@rm -f $(OBJ) test_cgi_standalone simple_cgi_test

fclean: clean
	@rm -f $(NAME)
	@rm -rf conf/*ServerConfig*
	@rm -rf conf/*locationConfig*
	@rm -rf conf/*tempConfig*

re: fclean all

# CGI Testing targets
test-cgi: 
	@echo "🧪 Testing CGI system independently..."
	@python3 validate_cgi.py

test-cgi-standalone:
	@echo "🔧 Compiling standalone CGI test..."
	@$(CC) $(CXXFLAGS) test_cgi_standalone.cpp src/CGIExecutor.cpp -o test_cgi_standalone
	@echo "🚀 Running standalone CGI test..."
	@./test_cgi_standalone
	@rm -f test_cgi_standalone

test-cgi-full: test-cgi-standalone test_cgi_independent.sh validate_cgi.py
	@echo "🧪 Running comprehensive CGI test suite..."
	@./test_cgi_independent.sh
	@python3 validate_cgi.py

simple-cgi-test: simple_cgi_test.cpp
	@echo "🔧 Compiling simple CGI test..."
	@$(CXX) $(CXXFLAGS) -o simple_cgi_test simple_cgi_test.cpp
	@echo "🚀 Running simple CGI test..."
	@./simple_cgi_test

.PHONY: all clean fclean make test-cgi test-cgi-standalone test-cgi-full