#include "ClientController.h"

int main(int argc, char* argv[]) {
	boost::asio::io_service io_service;


	try {
		ClientController *clientController = new ClientController(io_service);
		clientController->start();
		io_service.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}