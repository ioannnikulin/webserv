CPP = c++
# sometimes gets used implicitly, so safer to define
CXX = ${CPP}
COMPILE_FLAGS = -Wall -Wextra -Werror -std=c++98 -g -pedantic -Wold-style-cast -Wdeprecated-declarations
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

CONFIG_F = configuration
CONFIG_SRC_NAMES = \
	AppConfig.cpp \
	Endpoint.cpp \
	RouteConfig.cpp \
	FolderConfig.cpp \
	CgiHandlerConfig.cpp \
	UploadConfig.cpp \


CONFIG_SRCS = $(addprefix $(SOURCE_F)/$(CONFIG_F)/,$(CONFIG_SRC_NAMES))

# ------------------------------------------------------------

HTTP_METHODS_F = http_methods
HTTP_METHODS_SRC_NAMES =
HTTP_METHODS_SRCS = $(addprefix $(SOURCE_F)/$(HTTP_METHODS_F)/,$(HTTP_METHODS_SRC_NAMES))

# ------------------------------------------------------------
MAIN_NONENDPOINT_SRCS = \
	$(RESPONSE_GENERATOR_SRCS) \
	$(CONFIG_SRCS) \

MAIN_NONENDPOINT_OBJS = $(addprefix $(OBJ_F)/,$(MAIN_NONENDPOINT_SRCS:.cpp=.o))

# ------------------------------------------------------------

MAIN_ENDPOINT_SRC_NAME = webserv.cpp
MAIN_ENDPOINT_SRC = $(SOURCE_F)/$(MAIN_ENDPOINT_SRC_NAME)
MAIN_ENDPOINT_OBJ = $(OBJ_F)/$(MAIN_ENDPOINT_SRC:.cpp=.o)
MAIN_FNAME = webserv

MAIN_DIRS = \
	$(SOURCE_F) \
	$(SOURCE_F)/$(RESPONSE_GENERATOR_F) \
	$(SOURCE_F)/$(CONFIG_F) \
	$(SOURCE_F)/$(HTTP_METHODS_F)

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

LINK_FLAGS = \
	-I$(SOURCE_F) \
	-I$(TEST_F) \
	-I$(SOURCE_F)/$(HTTP_METHODS_F) \
	-I$(SOURCE_F)/$(RESPONSE_GENERATOR_F) \
	-I$(SOURCE_F)/$(CONFIG_F)
	
vpath %.cpp \
	$(SOURCE_F) \
	$(SOURCE_F)/$(HTTP_METHODS_F) \
	$(SOURCE_F)/$(RESPONSE_GENERATOR_F) \
	$(SOURCE_F)/$(CONFIG_F) \
	$(TEST_F) \
	$(TEST_F)/unit \
	$(TEST_F)/e2e

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

test: run-tests

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

LINTERS_F = linters

# ------------------------------------------------------------

external-calls:
	@bash ${LINTERS_F}/external_calls.sh $(OBJ_F)/$(SOURCE_F)

# ------------------------------------------------------------

# run to fix simple formatting issues
# see .clang-format for settings
format-fix:
	@bash ${LINTERS_F}/clang_format.sh fix ${SOURCE_F} ${TEST_F}

# runs fixer and compares with original,
# exits with 1 if any file would change
format-check:
	@bash ${LINTERS_F}/clang_format.sh check ${SOURCE_F} ${TEST_F}

# ------------------------------------------------------------

# smarter analysis: bugs, potential undefined behavior
# no autofixes
cppcheck:
	@bash ${LINTERS_F}/cppcheck.sh c++98 "$(LINK_FLAGS)" $(SOURCE_F) $(TEST_F)

# ------------------------------------------------------------

# smartest analysis, see .clang-tidy file for settings
tidy-check:
	@bash ${LINTERS_F}/clang_tidy.sh check "$(COMPILE_FLAGS)" "$(LINK_FLAGS)" $(SOURCE_F)

# will fix something, but not everything
# e.g. if you have too many local variables, will not split the function automatically
tidy-fix:
	@bash ${LINTERS_F}/clang_tidy.sh fix "$(COMPILE_FLAGS)" "$(LINK_FLAGS)" $(SOURCE_F)

# ------------------------------------------------------------

# custom scripts for conventions that are missing from standard solutions
header-check:
	@python3 ${LINTERS_F}/header_checker.py sources include tests

source-check:
	@python3 ${LINTERS_F}/source_checker.py sources include tests

makefile-check:
	@python3 ${LINTERS_F}/makefile_checker.py . Makefile
# ------------------------------------------------------------

# run in 42 campus
# skips some checks whose prerequisites cannot be installed
# since we don't have root access
test-campus: external-calls format-check header-check source-check run-tests
	@echo "CLEAN"

# runs in GitHub Actions environment, use on personal machine too
test-github: external-calls format-check cppcheck tidy-check header-check source-check run-tests
	@echo "CLEAN"

# ------------------------------------------------------------

%:
	@echo "What do you mean, '$@'?"

# ------------------------------------------------------------

.PHONY: all clean fclean re run-tests test \
	external-calls \
	format-fix format-check \
	cppcheck \
	tidy-check tidy-fix \
	header-check source-check makefile-check \
	test-campus test-github