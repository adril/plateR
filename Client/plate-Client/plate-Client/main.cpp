#include "Client.hpp"
#include "AppData.hpp"

int main(int argc, char* argv[]) {
	try {
		boost::asio::io_service io_service;
		tcp::resolver r(io_service);
		Client c(io_service);

		std::string host = AppData::getInstance()._host;
		std::string port = AppData::getInstance()._port;
		c.start(r.resolve(tcp::resolver::query(host, port)));

		io_service.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}