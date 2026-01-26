#ifndef STARTUPVALIDATIONTESTS_HPP
#define STARTUPVALIDATIONTESTS_HPP

#include <cxxtest/TestSuite.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "logger/LoggerConfig.hpp"
#include "request_handler/GetHandler.hpp"

using std::map;
using std::ofstream;
using std::set;
using std::string;
using std::vector;

class StartupValidationTests : public CxxTest::TestSuite {
private:
    static string _testRoot;
    static map<string, string> _files;

    static const string BAD_CONFIGS_DIR;
    static const string NO_PERMISSION_FILE;
    static const string STATUS_500_FILE;

    mode_t _originalPermNoPermission;
    mode_t _originalPermStatus500;

    // Helper to save original permissions
    mode_t getFilePermissions(const string& filepath) {
        struct stat fileStat;
        if (stat(filepath.c_str(), &fileStat) == 0) {
            return fileStat.st_mode;
        }
        return 0;
    }

    // Helper to change permissions
    bool setFilePermissions(const string& filepath, mode_t mode) {
        return chmod(filepath.c_str(), mode) == 0;
    }

public:
    void setUp() {
        // Save original permissions before changing them
        _originalPermNoPermission = getFilePermissions(NO_PERMISSION_FILE);
        _originalPermStatus500 = getFilePermissions(STATUS_500_FILE);

        // Remove read permissions from the files
        setFilePermissions(NO_PERMISSION_FILE, 0000);
        setFilePermissions(STATUS_500_FILE, 0000);

        webserver::LoggerConfig::setGlobalLevel(LOG_SILENT);
    }

    void tearDown() {
        // Restore original permissions
        setFilePermissions(NO_PERMISSION_FILE, _originalPermNoPermission);
        setFilePermissions(STATUS_500_FILE, _originalPermStatus500);
    }

    void testBadConfigFilesThrowExceptions() {
        vector<string> badConfigs;

        // List all bad config files explicitly
        badConfigs.push_back(BAD_CONFIGS_DIR + "/2_empty_file.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/10_missing_curly_brace.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/11_extra_curly_brace.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/12_unclosed_block.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/13_empty_block.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/14_directive_outside_allowed_context.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/15_missing_semicolon.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/16_extra_semicolon.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/18_unknown_directive.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/19_wrong_number_of_arguments.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/20_invalid_value_type.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/21_case-sensitive_directive_names.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/29_no_listen.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/30_invalid_port.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/31_port_too_large.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/33_duplicate_ip_port_exact_match.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/34_listen_in_wrong_scope.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/35_empty_server_name.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/36_duplicate_names_in_same_server.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/40_location_outside_server.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/41_location_inside_location.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/42_duplicate_location_paths.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/43_empty_location_block.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/45_root_does_not_exist.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/46_root_is_file.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/47_no_read_permission.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/50_upload_dir_missing.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/51_cgi_missing.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/59_duplicate_methods.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/69_client_body_size_zero.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/70_client_body_size_negative.conf");
        badConfigs.push_back(
            BAD_CONFIGS_DIR + "/71_client_body_size_multiple_definitions_same_scope.conf"
        );

        webserver::ConfigParser parser;

        for (size_t i = 0; i < badConfigs.size(); ++i) {
            TS_ASSERT_THROWS(parser.parse(badConfigs[i]), const webserver::ConfigParsingException&);
        }
    }

    void testGoodConfigFilesDontThrowExceptions() {
        vector<string> goodConfigs;

        goodConfigs.push_back(BAD_CONFIGS_DIR + "/9_tabs_spaces_mix.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/17_comment_handling.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/48_index_missing.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/49_error_page_missing.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/60_no_methods_allowed.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/63_invalid_http_code.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/64_duplicate_same_code.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/65_path_missing.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/66_path_directory.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/67_permission_denied.conf");
        goodConfigs.push_back(BAD_CONFIGS_DIR + "/68_client_body_size_missing.conf");

        webserver::ConfigParser parser;

        for (size_t i = 0; i < goodConfigs.size(); ++i) {
            TS_ASSERT_THROWS_NOTHING(parser.parse(goodConfigs[i]));
        }
    }
};

// Static member initialization
string StartupValidationTests::_testRoot = "tests/unit/volume";
map<string, string> StartupValidationTests::_files;
const string StartupValidationTests::BAD_CONFIGS_DIR = "tests/unit/bad_config_files";
const string StartupValidationTests::NO_PERMISSION_FILE = "tests/unit/volume/no_permission.html";
const string StartupValidationTests::STATUS_500_FILE = "tests/unit/volume/status_pages/500.html";

#endif
