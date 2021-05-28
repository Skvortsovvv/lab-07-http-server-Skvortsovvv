#include <header.hpp>
int main() {
    Suggestion SG;
	boost::asio::io_service io_service;
	tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));
    std::thread(update_suggestion, std::ref(SG)).detach();
	for (;;) {
		tcp::socket socket(io_service);
		acceptor.accept(socket);
		std::cout << "Connected" << std::endl;
		std::thread(do_session, std::move(socket), std::ref(SG)).detach();
	}
}
