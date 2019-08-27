#include "Analyzer.h"
#include <boost/chrono.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc < 3) 
	{
		std::cout << "Specify path to input file with the text and path to output file" << std::endl;
		return 1;
	}
	try
	{
	    boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
	    Analyzer analyzer;
	    analyzer.analyze_text(argv[1]);
	    analyzer.write_data_to_file(argv[2]);
	    boost::chrono::duration<double> sec = boost::chrono::system_clock::now() - start;
	    std::cout << "took " << sec.count() << " seconds\n";
	}
	catch (const std::runtime_error& error)
	{
		std::cout << error.what() << std::endl;
	}
}
