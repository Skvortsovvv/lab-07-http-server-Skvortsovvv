// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include "boost/array.hpp"
#include "boost/asio.hpp"
#include "nlohmann/json.hpp"
#include "beast/core.hpp"
#include "beast/config.hpp"
#include "beast/http.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <thread>
#include <fstream>
#include <chrono>
#include <shared_mutex>
using json = nlohmann::json;
using boost::asio::ip::tcp;

struct Suggestion {
	std::shared_mutex mute;
	json suggestions;
	Suggestion();
	void update_suggestions();
	json suggest(std::string& input);
};

struct Data_Handler {
	std::string data_body;
	std::ofstream ofs;
	void file_save(std::string& input_data);
	void body_parser();
};

class Response {
public:
	Response() {}

	Response& AddHeader(const std::string& name, std::string value);

	Response& SetCode(int code);

	Response& SetBody(std::string body);

	boost::system::error_code SendTo(tcp::socket& socket);

private:
	const char* Code() {
		if (code_ == 200)
			return "OK";
		else
			return "";
	}
	std::map<std::string, std::string> headers_;
    int code_ = 0;
    std::string body_;
};

void update_suggestion(Suggestion& sg);

void do_session(tcp::socket& socket, Suggestion& SG);


#endif // INCLUDE_HEADER_HPP_
