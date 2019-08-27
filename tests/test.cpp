#define BOOST_TEST_MODULE Analyzer

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "Analyzer.h"
#include <string>
#include <fstream>
#include <iostream>

BOOST_AUTO_TEST_SUITE(InputFile)

    BOOST_AUTO_TEST_CASE(File_is_not_exist)
    {
        Analyzer analyzer;
        BOOST_CHECK_THROW (analyzer.analyze_text("/usr/not_exist.txt"), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(File_is_empty)
    {
        Analyzer analyzer;
        {
            std::ofstream file("test_file.txt");
        }
        BOOST_CHECK_THROW (analyzer.analyze_text("./test_file.txt"), std::runtime_error);
        boost::filesystem::remove("./test_file.txt");
    }

BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(OutputFile)

    BOOST_AUTO_TEST_CASE(Dir_is_not_exists)
    {
        Analyzer analyzer;
        BOOST_CHECK_THROW (analyzer.write_data_to_file("../some/not_exist.txt"), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(File_could_not_be_opened)
    {
        Analyzer analyzer;
        {
            std::ofstream file("test_file.txt");
            file << "some";
        }
        using namespace boost::filesystem;
        permissions("./test_file.txt", owner_read|others_read);
        BOOST_CHECK_THROW (analyzer.write_data_to_file("./test_file.txt"), std::runtime_error);
        boost::filesystem::remove("./test_file.txt");
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Analyzer_test)

    BOOST_AUTO_TEST_CASE(Normal_text)
    {
        Analyzer analyzer;
        {
            std::ofstream test_file("test_file.txt");
            test_file << "some test words for count\n more test words for count";
        }
        analyzer.analyze_text("test_file.txt");
        analyzer.write_data_to_file("./output_file.txt");
        {
            std::ifstream output_file("output_file.txt");
            std::string output;
            while (!output_file.eof())
            {
                std::string temp;
                std::getline(output_file, temp);
                output += temp;
            }
            BOOST_CHECK(output == "2 count2 for2 test2 words1 more1 some");
            boost::filesystem::remove("./test_file.txt");
            boost::filesystem::remove("./output_file.txt");
        }
    }

    BOOST_AUTO_TEST_CASE(No_letters)
    {
        Analyzer analyzer;
        {
            std::ofstream test_file("test_file.txt");
            test_file << " \n \n 1 2 3 41243 \n 123 \n 32122 ";
        }
        analyzer.analyze_text("test_file.txt");
        analyzer.write_data_to_file("./output_file.txt");
        {
            std::ifstream output_file("output_file.txt");
            std::string output;
            while (!output_file.eof())
            {
                std::string temp;
                std::getline(output_file, temp);
                output += temp;
            }
            BOOST_CHECK(output == "");
            boost::filesystem::remove("./test_file.txt");
            boost::filesystem::remove("./output_file.txt");
        }
    }

BOOST_AUTO_TEST_SUITE_END()
