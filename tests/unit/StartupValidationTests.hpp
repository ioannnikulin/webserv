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
#include "configuration/parser/ConfigParsingException.hpp"
#include "http_methods/HttpMethodType.hpp"
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
    }

    void tearDown() {
        // Restore original permissions
        setFilePermissions(NO_PERMISSION_FILE, _originalPermNoPermission);
        setFilePermissions(STATUS_500_FILE, _originalPermStatus500);
    }

    void testBadConfigFilesThrowExceptions() {
        vector<string> badConfigs;

        // List all bad config files explicitly
        badConfigs.push_back(BAD_CONFIGS_DIR + "/45_root_does_not_exist.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/46_root_is_file.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/47_no_read_permission.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/48_index_missing.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/49_error_page_missing.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/50_upload_dir_missing.conf");
        badConfigs.push_back(BAD_CONFIGS_DIR + "/51_cgi_missing.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/63_invalid_http_code.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/64_duplicate_same_code.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/65_path_missing.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/66_path_directory.conf");
        //badConfigs.push_back(BAD_CONFIGS_DIR + "/67_permission_denied.conf");

        //TS_ASSERT_EQUALS(12, badConfigs.size());

        webserver::ConfigParser parser;

        for (size_t i = 0; i < badConfigs.size(); ++i) {
            TS_ASSERT_THROWS(parser.parse(badConfigs[i]), const webserver::ConfigParsingException&);
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
