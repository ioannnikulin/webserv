CPP = c++
COMPILE_FLAGS = -Wall -Wextra -Werror -std=c++98 -g -pedantic -c
LINK_FLAGS = 
PREPROC_DEFINES = 

SOURCE_F = sources
TEST_F = tests
OBJ_F = build
TEST_OBJ_F = $(OBJ_F)/$(TEST_F)

# ------------------------------------------------------------

RESPONSE_GENERATOR_F = response_generator
RESPONSE_GENERATOR_SRC_NAMES = response_generator.cpp
RESPONSE_GENERATOR_SRCS = $(addprefix $(SOURCE_F)/$(RESPONSE_GENERATOR_F)/,$(RESPONSE_GENERATOR_SRC_NAMES))
RESPONSE_GENERATOR_OBJS = $(addprefix $(OBJ_F)/,$(RESPONSE_GENERATOR_SRCS:.cpp=.o))

# ------------------------------------------------------------

# other sections

# ------------------------------------------------------------

MAIN_NONENDPOINT_OBJS = $(RESPONSE_GENERATOR_OBJS)

# ------------------------------------------------------------

MAIN_ENDPOINT_SRC_NAME = webserv.cpp
MAIN_ENDPOINT_SRC = $(SOURCE_F)/$(MAIN_ENDPOINT_SRC_NAME)
MAIN_ENDPOINT_OBJ = $(OBJ_F)/$(MAIN_ENDPOINT_SRC:.cpp=.o)
MAIN_FNAME = webserv

MAIN_DIRS = $(SOURCE_F) $(SOURCE_F)/$(RESPONSE_GENERATOR_F)

MAIN_OBJ_DIRS = $(addprefix $(OBJ_F)/, $(MAIN_DIRS))

# ------------------------------------------------------------

TEST_ENDPOINT_SRC_NAME = main_test.cpp
TEST_ENDPOINT_SRC = $(TEST_F)/$(TEST_ENDPOINT_SRC_NAME)
TEST_ENDPOINT_OBJ = $(OBJ_F)/$(TEST_ENDPOINT_SRC:.cpp=.o)
TEST_FNAME = $(TEST_F)/main_test

TEST_SRC_NAMES = unit/unit_tests.cpp e2e/e2e_tests.cpp
TEST_SRCS = $(addprefix $(TEST_F)/,$(TEST_SRC_NAMES))
TEST_OBJS = $(addprefix $(OBJ_F)/,$(TEST_SRCS:.cpp=.o))

# ------------------------------------------------------------

vpath %.cpp $(SOURCE_F) $(TEST_F)

all: $(MAIN_FNAME)

$(MAIN_FNAME): $(MAIN_ENDPOINT_OBJ) | $(OBJ_F) $(MAIN_OBJ_DIRS)
	$(CPP) $(LINK_FLAGS) $^ -o $@

$(MAIN_ENDPOINT_OBJ): $(MAIN_ENDPOINT_SRC) | $(OBJ_F) $(MAIN_OBJ_DIRS)
	$(CPP) $(COMPILE_FLAGS) $(PREPROC_DEFINES) $< -o $@

$(OBJ_F): #ensure it exists
	mkdir -p $@

$(MAIN_OBJ_DIRS): | $(OBJ_F)
	mkdir -p $@

# ------------------------------------------------------------

clean:
	rm -rf $(OBJ_F) $(TEST_FNAME)

fclean: clean
	rm -f $(MAIN_FNAME)

re: fclean all

# ------------------------------------------------------------

ALLOWED_EXTERNAL_FUNCTIONS = execve pipe strerror gai_strerror errno dup dup2 fork socketpair htons htonl ntohs ntohl select poll epoll epoll_create epoll_ctl epoll_wait kqueue kqueue kevent socket accept listen send recv chdir bind connect getaddrinfo freeaddrinfo setsockopt getsockname getprotobyname fcntl close read write waitpid kill signal access stat open opendir readdir closedir __cxa_atexit __dso_handle _GLOBAL_OFFSET_TABLE_

external_calls:
	@printf "%s\n" $(ALLOWED_EXTERNAL_FUNCTIONS) > allowed.txt
	@make >/dev/null
	@# collect undefined symbol names (last field from nm -u)
	@find $(OBJ_F) -name "*.o" -exec nm -u {} + | awk '{print $$NF}' | sort -u > all_calls.txt
	@# filter out allowed
	@grep -v '^_Z' all_calls.txt | grep -vFf allowed.txt > forbidden_calls.txt || true
	@if [ -s forbidden_calls.txt ]; then \
		echo "Error: Forbidden external calls detected:"; \
		cat forbidden_calls.txt; \
		rm -f allowed.txt all_calls.txt forbidden_calls.txt; \
		exit 1; \
	fi
	@rm -f allowed.txt all_calls.txt forbidden_calls.txt

# ------------------------------------------------------------

.PHONY: all clean fclean re external_calls