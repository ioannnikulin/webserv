CPP = c++
# sometimes gets used implicitly, so safer to define
CXX = ${CPP}
COMPILE_FLAGS = -Wall -Wextra -Werror	\
				-std=c++98	\
				-g	\
				-pedantic -Wold-style-cast -Wdeprecated-declarations  \

PREPROC_DEFINES =

SOURCE_F = sources
TEST_F = tests
OBJ_F = build
TEST_OBJ_F = $(OBJ_F)/$(TEST_F)

# ------------------------------------------------------------

APP_CONFIG_F = configuration
APP_CONFIG_SRC_NAMES = \
	AppConfig.cpp \
	Endpoint.cpp \
	RouteConfig.cpp \
	FolderConfig.cpp \
	CgiHandlerConfig.cpp \
	UploadConfig.cpp \


APP_CONFIG_SRCS = $(addprefix $(SOURCE_F)/$(APP_CONFIG_F)/,$(APP_CONFIG_SRC_NAMES))

# ------------------------------------------------------------

APP_CONFIG_PARSER_F = $(APP_CONFIG_F)/parser
APP_CONFIG_PARSER_SRC_NAMES = \
	ConfigParser.cpp \
	ConfigParserElements.cpp \
	ConfigParserLocation.cpp \
	ConfigParserTokens.cpp \
	LocationTempData.cpp \

APP_CONFIG_PARSER_SRCS = $(addprefix $(SOURCE_F)/$(APP_CONFIG_PARSER_F)/,$(APP_CONFIG_PARSER_SRC_NAMES))

# ------------------------------------------------------------

LISTENER_F = listener
LISTENER_SRC_NAMES = Listener.cpp MasterListener.cpp
LISTENER_SRCS = $(addprefix $(SOURCE_F)/$(LISTENER_F)/,$(LISTENER_SRC_NAMES))

# ------------------------------------------------------------

CONNECTION_F = connection
CONNECTION_SRC_NAMES = Connection.cpp
CONNECTION_SRCS = $(addprefix $(SOURCE_F)/$(CONNECTION_F)/,$(CONNECTION_SRC_NAMES))

# ------------------------------------------------------------

REQUEST_F = request
REQUEST_SRC_NAMES = Request.cpp
REQUEST_SRCS = $(addprefix $(SOURCE_F)/$(REQUEST_F)/,$(REQUEST_SRC_NAMES))

# ------------------------------------------------------------

REQUEST_HANDLER_F = request_handler
REQUEST_HANDLER_SRC_NAMES = RequestHandler.cpp GetHandler.cpp
REQUEST_HANDLER_SRCS = $(addprefix $(SOURCE_F)/$(REQUEST_HANDLER_F)/,$(REQUEST_HANDLER_SRC_NAMES))

# ------------------------------------------------------------

RESPONSE_F = response
RESPONSE_SRC_NAMES = Response.cpp
RESPONSE_SRCS = $(addprefix $(SOURCE_F)/$(RESPONSE_F)/,$(RESPONSE_SRC_NAMES))

# ------------------------------------------------------------
FILE_SYSTEM_F = file_system
FILE_SYSTEM_SRC_NAMES = FileSystem.cpp MimeTypes.cpp
FILE_SYSTEM_SRCS = $(addprefix $(SOURCE_F)/$(FILE_SYSTEM_F)/,$(FILE_SYSTEM_SRC_NAMES))

# ------------------------------------------------------------

HTTP_METHODS_F = http_methods
HTTP_METHODS_SRC_NAMES = HttpMethodType.cpp
HTTP_METHODS_SRCS = $(addprefix $(SOURCE_F)/$(HTTP_METHODS_F)/,$(HTTP_METHODS_SRC_NAMES))

# ------------------------------------------------------------

HTTP_STATUSES_F = http_status
HTTP_STATUSES_SRC_NAMES = HttpStatus.cpp HttpException.cpp
HTTP_STATUSES_SRCS = $(addprefix $(SOURCE_F)/$(HTTP_STATUSES_F)/,$(HTTP_STATUSES_SRC_NAMES))

# ------------------------------------------------------------

UTILS_F = utils
UTILS_SRC_NAMES = utils.cpp
UTILS_SRCS = $(addprefix $(SOURCE_F)/$(UTILS_F)/,$(UTILS_SRC_NAMES))

# ------------------------------------------------------------

WEBSERV_SRC_NAMES = WebServer.cpp
WEBSERV_SRCS = $(addprefix $(SOURCE_F)/,$(WEBSERV_SRC_NAMES))

# ------------------------------------------------------------

MAIN_NONENDPOINT_SRCS = \
	$(APP_CONFIG_SRCS) \
	$(APP_CONFIG_PARSER_SRCS) \
	$(HTTP_METHODS_SRCS) \
	$(LISTENER_SRCS) \
	$(CONNECTION_SRCS) \
	$(REQUEST_SRCS) \
	$(REQUEST_HANDLER_SRCS) \
	$(RESPONSE_SRCS) \
	$(WEBSERV_SRCS) \
	$(HTTP_METHODS_SRCS) \
	$(HTTP_STATUSES_SRCS) \
	$(FILE_SYSTEM_SRCS) \
	$(UTILS_SRCS) \
	
MAIN_NONENDPOINT_OBJS = $(addprefix $(OBJ_F)/,$(MAIN_NONENDPOINT_SRCS:.cpp=.o))

# ------------------------------------------------------------

MAIN_ENDPOINT_SRC_NAME = webserv.cpp
MAIN_ENDPOINT_SRC = $(SOURCE_F)/$(MAIN_ENDPOINT_SRC_NAME)
MAIN_ENDPOINT_OBJ = $(OBJ_F)/$(MAIN_ENDPOINT_SRC:.cpp=.o)
MAIN_FNAME = webserv

MAIN_DIRS = \
	$(SOURCE_F) \
	$(SOURCE_F)/$(APP_CONFIG_F) \
	$(SOURCE_F)/$(APP_CONFIG_PARSER_F) \
	$(SOURCE_F)/$(LISTENER_F) \
	$(SOURCE_F)/$(HTTP_METHODS_F) \
	$(SOURCE_F)/$(CONNECTION_F) \
	$(SOURCE_F)/$(REQUEST_F) \
	$(SOURCE_F)/$(REQUEST_HANDLER_F) \
	$(SOURCE_F)/$(RESPONSE_F) \
	$(SOURCE_F)/$(HTTP_STATUSES_F) \
	$(SOURCE_F)/$(FILE_SYSTEM_F) \
	$(SOURCE_F)/$(UTILS_F) \


MAIN_OBJ_DIRS = $(addprefix $(OBJ_F)/, $(MAIN_DIRS))

# ------------------------------------------------------------

LINK_FLAGS = \
	-I$(TEST_F) \
	-I$(SOURCE_F) \


# for makefile only: where to look for cpps mentioned in rules
vpath %.cpp \
	$(MAIN_DIRS) \
	$(TEST_F) \
	$(TEST_F)/unit \
	$(TEST_F)/e2e \


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

CXXTEST_F=cxxtest
CXXTEST_ZIP=$(CXXTEST_F)/master.zip
TEST_EXECUTABLE=test

install-cxxtest:
	@if [ ! -d "$(CXXTEST_F)" ]; then \
		mkdir -p $(CXXTEST_F); \
		wget -qO "$(CXXTEST_ZIP)" https://github.com/CxxTest/cxxtest/archive/refs/heads/master.zip; \
		unzip -qq "$(CXXTEST_ZIP)" -d "$(CXXTEST_F)"; \
		mv $(CXXTEST_F)/cxxtest-master/* $(CXXTEST_F); \
		rm -rf $(CXXTEST_F)/cxxtest-master $(CXXTEST_F)/master.zip;\
	fi

TEST_HEADERS := $(shell find tests -name "*.hpp")

generate-cxxtest-tests: | $(OBJ_F)
	@echo "Generating tests from $(TEST_HEADERS)"
# syntax warnings come from internal cxxtest generator code problems, hiding
	@PYTHONWARNINGS="ignore::SyntaxWarning" python3 $(CXXTEST_F)/bin/cxxtestgen --error-printer -o $(OBJ_F)/cxx_runner.cpp $(TEST_HEADERS)

build-cxxtest-tests: $(MAIN_NONENDPOINT_OBJS)
	@$(CPP) -std=c++98 -I$(CXXTEST_F) $(LINK_FLAGS) -o $(TEST_EXECUTABLE) $(OBJ_F)/cxx_runner.cpp $^

test: install-cxxtest generate-cxxtest-tests build-cxxtest-tests
	@./$(TEST_EXECUTABLE)

# ------------------------------------------------------------

clean:
	rm -rf $(OBJ_F) $(TEST_FNAME) $(CXXTEST_F)

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
	@if python3 linters/is_clang_available_in_python.py; then \
		python3 ${LINTERS_F}/header_checker.py $(SOURCE_F) include; \
	else \
		python3 ${LINTERS_F}/campus/header_checker.py $(SOURCE_F) include; \
	fi

source-check:
	@if python3 linters/is_clang_available_in_python.py; then \
		python3 ${LINTERS_F}/source_checker.py $(SOURCE_F) include tests; \
	else \
		python3 ${LINTERS_F}/campus/source_checker.py $(SOURCE_F) include tests; \
	fi

makefile-check:
	@python3 ${LINTERS_F}/makefile_checker.py $(SOURCE_F) Makefile

# ------------------------------------------------------------

# run in 42 campus
# skips some checks whose prerequisites cannot be installed
# since we don't have root access
test-campus: external-calls format-check header-check source-check makefile-check test
	@echo "CLEAN"

# runs in GitHub Actions environment, use on personal machine too
test-github: external-calls format-check cppcheck tidy-check header-check source-check makefile-check test
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
	install-cxxtest generate-cxxtest-tests build-cxx-tests \
	test-campus test-github \

