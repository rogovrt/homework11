#include <iostream>
#include <string>
#include <thread>
#include <atomic>

#include <boost/asio.hpp>

std::string user_name;
std::atomic <bool> b(true);

void write_data(boost::asio::ip::tcp::socket& socket) {
	std::string data;
	while (b) {
		std::getline(std::cin, data);
		if (data == "EXIT") {
			b = false;
		}
		data.insert(0, user_name);
		data.push_back('\n');
		boost::asio::write(socket, boost::asio::buffer(data));
	}
}

void read_data(boost::asio::ip::tcp::socket& socket) {
	std::string data;
	while (b && data.find_first_of("EXIT") == std::string::npos) {
		boost::asio::streambuf buffer;
		boost::asio::read_until(socket, buffer, '\n');
		std::istream input_stream(&buffer);
		std::getline(input_stream, data, '\n');
		std::cout << data << std::endl;
	}
}

int main() {
	system("chcp 1251");
	std::cout << "Enter your nickname: ";
	std::getline(std::cin, user_name);
	if (!user_name.empty())
		user_name.push_back(':');
	system("pause");

	const std::string ip_address = "127.0.0.1";
	const auto port = 3333;
	try {
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(ip_address), port
		);

		boost::asio::io_service ios;
		boost::asio::ip::tcp::socket socket(ios, endpoint.protocol());
		socket.connect(endpoint);

		std::thread t(write_data, std::ref(socket));
		std::thread t1(read_data, std::ref(socket));
		t.join();
		t1.join();
	}
	catch (boost::system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;
		system("pause");
		return e.code().value();
	}
	system("pause");

	return 0;
}