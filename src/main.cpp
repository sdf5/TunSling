#include <iostream>
#include "cNode_factory.h"
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	try {
		// Declare the supported options.
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "Produce help message")
			("address", po::value<std::string>()->required(), "Address (required)")
			("threads", po::value<int>()->required(), "Number threads, if 0 then disable threadpool (required)")
			("UDP", po::value<std::string>()->required(), "UDP type (Asio, Sendmmsg) (required)")
			("crypto", po::value<std::string>()->required(), "Crypto type (Secretbox, X_salsa_20, Empty) (required)")
			("tun", po::value<std::string>()->required(), "Tun type (LinuxNormal, LinuxWeld) (required)")
			("tunMtu", po::value<int>()->required(), "MTU size to set on our Tun (typial 9000, 8972, 1500, 1472) (required)")
		;
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		
		if (vm.count("help")) {
			cout << desc << "\n";
			return 0;
		}
		
		po::notify(vm);
		
		cNode_factory factory;
		auto my_node = factory.create_node( vm );
		my_node->run();
	} catch (std::exception &e) {
		std::cout << e.what() << "\n";
	} catch ( ... ) {
		std::cout << "Uknown error." << "\n";
	}
			
    return 0;
}
