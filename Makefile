CPP = c++
# sometimes gets used implicitly, so safer to define
CXX = ${CPP}
COMPILE_FLAGS = -Wall -Wextra -Werror -std=c++98 -g -pedantic -Wold-style-cast -Wdeprecated-declarations
LINK_FLAGS = -I$(SOURCE_F) -I$(TEST_F)
PREPROC_DEFINES = 

SOURCE_F = sources
TEST_F = tests
OBJ_F = build
TEST_OBJ_F = $(OBJ_F)/$(TEST_F)

# ------------------------------------------------------------

RESPONSE_GENERATOR_F = response_generator
RESPONSE_GENERATOR_SRC_NAMES = response_generator.cpp
RESPONSE_GENERATOR_SRCS = $(addprefix $(SOURCE_F)/$(RESPONSE_GENERATOR_F)/,$(RESPONSE_GENERATOR_SRC_NAMES))

# ------------------------------------------------------------

# other sections

# ------------------------------------------------------------

MAIN_NONENDPOINT_SRCS = $(RESPONSE_GENERATOR_SRCS)
MAIN_NONENDPOINT_OBJS = $(addprefix $(OBJ_F)/,$(MAIN_NONENDPOINT_SRCS:.cpp=.o))

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
TEST_FNAME = main_test

TEST_SRC_NAMES = unit/runUnitTests.cpp e2e/runEndToEndTests.cpp
TEST_SRCS = $(addprefix $(TEST_F)/,$(TEST_SRC_NAMES))
TEST_OBJS = $(addprefix $(OBJ_F)/,$(TEST_SRCS:.cpp=.o))

TEST_DIRS = $(TEST_F) $(TEST_F)/unit $(TEST_F)/e2e
TEST_OBJ_DIRS = $(addprefix $(OBJ_F)/, $(TEST_DIRS))

# ------------------------------------------------------------

# where to look for cpp files when managing makefile rules
vpath %.cpp $(SOURCE_F) $(TEST_F)

all: $(MAIN_FNAME)

# MAIN_ENDPOINT - main function of webserv, MAIN_NONENDPOINT - all other components except main
# compare this to $(TEST_fname) rule below
# this rule builds webserv executable
$(MAIN_FNAME): $(MAIN_ENDPOINT_OBJ) $(MAIN_NONENDPOINT_OBJS) | $(OBJ_F) $(MAIN_OBJ_DIRS)
	@$(CPP) $(LINK_FLAGS) $^ -o $@

$(OBJ_F)/%.o: %.cpp | $(OBJ_F) $(MAIN_OBJ_DIRS) $(TEST_OBJ_DIRS)
	@$(CPP) $(COMPILE_FLAGS) $(LINK_FLAGS) -c $(PREPROC_DEFINES) $< -o $@

$(OBJ_F): #ensure it exists
	@mkdir -p $@

$(MAIN_OBJ_DIRS): | $(OBJ_F)
	@mkdir -p $@

# ------------------------------------------------------------

run-tests: $(TEST_FNAME)
	@./$(TEST_FNAME)

# this rule takes main function defined in tests (not webserv's main), all test objects
# and builds test executable, making all components of webserv available for instantiation
$(TEST_FNAME): $(TEST_ENDPOINT_OBJ) $(TEST_OBJS) $(MAIN_NONENDPOINT_OBJS) | $(OBJ_F) $(MAIN_OBJ_DIRS) $(TEST_OBJ_DIRS)
	@$(CPP) $(LINK_FLAGS) $^ -o $@

$(TEST_OBJ_DIRS): #ensure it exists
	@mkdir -p $@

# ------------------------------------------------------------

clean:
	rm -rf $(OBJ_F) $(TEST_FNAME)

fclean: clean
	rm -f $(MAIN_FNAME)

re: fclean all

# ------------------------------------------------------------

# allowed functions from the subject
# if this check fails, and you are absolutely sure 
# this function is allowed by the subject, 
# feel free to add it in a separate block
ALLOWED_EXTERNAL_FUNCTIONS = execve pipe strerror gai_strerror \
errno dup dup2 fork socketpair htons htonl ntohs ntohl select \
poll epoll epoll_create epoll_ctl epoll_wait kqueue kqueue \
kevent socket accept listen send recv chdir bind connect \
getaddrinfo freeaddrinfo setsockopt getsockname getprotobyname \
fcntl close read write waitpid kill signal access stat \
open opendir readdir closedir

# allowed C++ implicit stuff
ALLOWED_EXTERNAL_FUNCTIONS += __cxa_atexit __dso_handle \
_GLOBAL_OFFSET_TABLE_ __gxx_personality_v0 __stack_chk_fail \
_Unwind_Resume

external-calls:
	@rm -f allowed.txt all_calls.txt forbidden_calls.txt
	@printf "%s\n" $(ALLOWED_EXTERNAL_FUNCTIONS) > allowed.txt
	@# build all objects, suppress output
	@make >/dev/null
	@# collect undefined symbol names (last field from nm -u)
	@find $(OBJ_F)/$(SOURCE_F) -name "*.o" -exec nm -u {} + | sed -n 's/.* U //p' | sort -u > all_calls.txt
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

CLANG_FORMAT ?= clang-format

# run to fix simple formatting issues
# see .clang-format for settings
format-fix:
	find . -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 -n1 $(CLANG_FORMAT) -style=file -i

# runs fixer and compares with original,
# exits with 1 if any file would change
format-check:
	@echo "Running clang-format check..."
	@changed=0; \
	for f in $(shell find . -name '*.cpp' -o -name '*.hpp'); do \
		tmp=$$f.formatted.tmp; \
		$(CLANG_FORMAT) -style=file "$$f" > "$$tmp"; \
		if ! cmp -s "$$f" "$$tmp"; then \
			echo "$$f is malformatted"; changed=1; fi; \
		rm -f "$$tmp"; \
	done; \
	exit $$changed

# ------------------------------------------------------------

# smarter analysis: bugs, potential undefined behavior
# no autofixes
cppcheck:
	@echo "Running cppcheck..."
	@command -v cppcheck >/dev/null 2>&1 || { echo "cppcheck: not found"; exit 1; }
	@out=$$(mktemp); \
	@cppcheck --std=c++98 --enable=all --inconclusive --suppress=missingIncludeSystem $(LINK_FLAGS) $(TEST_F) $(SOURCE_F) --template='{file}:{line}:{column}:{severity}:{id}:{message}' >$$out 2>&1 || true; \
	if grep -q -E ':(error|warning|style):' $$out; then \
		echo "cppcheck found issues:"; \
		grep -n -E ':(error|warning|style):' $$out; \
		rm -f $$out; \
		exit 1; \
	else \
		echo "cppcheck: no issues"; \
		rm -f $$out; \
	fi

# ------------------------------------------------------------

# smartest analysis, see .clang-tidy file for settings
CLANG_TIDY ?= clang-tidy

# translation units
TUS := $(shell find $(SOURCE_F) $(TEST_F) -type f -name '*.cpp' | sort)

tidy-check:
	@echo "Running clang-tidy check..."
	@command -v $(CLANG_TIDY) >/dev/null 2>&1 || { echo "clang-tidy: not found"; exit 1; }
	@errs=0; \
	for f in $(TUS); do \
		out=$$(mktemp); \
		$(CLANG_TIDY) $$f -- $(COMPILE_FLAGS) $(LINK_FLAGS) >$$out 2>&1 || true; \
		if grep -q -E 'warning:|error:' $$out; then \
			printf "clang-tidy issues in %s:\n" "$$f"; \
			grep -n -E 'warning:|error:' $$out; \
			errs=1; \
		fi; \
		rm -f $$out; \
	done; \
	if [ $$errs -ne 0 ]; then exit 1; fi

# will fix something, but not everything
# e.g. if you have too many local variables, will not split the function automatically
tidy-fix:
	@for f in $(TUS); do \
		$(CLANG_TIDY) -fix -format-style=file $$f -- $(COMPILE_FLAGS) $(LINK_FLAGS) || true; \
	done

# ------------------------------------------------------------

# run in 42 campus
# skips some checks whose prerequisites cannot be installed
# since we don't have root access
test-campus: external-calls format-check run-tests
	@echo "CLEAN"

# runs in GitHub Actions environment, use on personal machine too
test-github: external-calls format-check cppcheck tidy-check run-tests
	@echo "CLEAN"

# ------------------------------------------------------------

%:
	@echo "What do you mean, '$@'?"

# ------------------------------------------------------------

.PHONY: all clean fclean re run-tests external-calls format-fix format-check cppcheck tidy-check tidy-fix test-campus test-github