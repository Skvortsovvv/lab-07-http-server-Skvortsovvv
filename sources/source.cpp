// Copyright 2020 Your Name <your_email>
#include <header.hpp>


Suggestion::Suggestion() {
	std::ifstream ifs("suggestions.json");
	suggestions = json::parse(ifs);
}


void Suggestion::update_suggestions() {
	for (;;) {
		std::this_thread::sleep_for(std::chrono::seconds(900));
		std::unique_lock lock(mute);
		std::ifstream ifs("suggestions.json");
		suggestions = json::parse(ifs);
		std::cout << "SUGGESTION FILE UPDATED" << '\n';
	}
}

json Suggestion::suggest(std::string& input) {
	std::shared_lock lock(mute);
	json response;
	response["suggestions"] = json::array();
	std::map<int, std::string> list;
	for (int i = 0; i < suggestions.size(); ++i) {
		if (input == suggestions[i].at("id")) {
			list[suggestions[i].at("cost")] = suggestions[i].at("name");
		}
		else {
			return response;
		}
	}
	int index = 0;
	for (const auto& [key, value] : list) {
		json obj = { {"text", value},{"position", index} };
		response["suggestions"].push_back(obj);
		++index;
	}
	return response;
}

void Data_Handler::file_save(std::string& input_data) {
	ofs.open("input.json");
	ofs << input_data;
	ofs.close();
}

void Data_Handler::body_parser() {
	std::ifstream ifs("input.json");
	json js = json::parse(ifs);
	data_body = js.at("input");
}


Response& Response::AddHeader(const std::string& name, std::string value) {
	headers_[name] = std::move(value);
	return *this;
}

Response& Response::SetCode(int code) {
	code_ = code;
	return *this;
}

Response& Response::SetBody(std::string body) {
	body_ = std::move(body);
	AddHeader("Content-Length", std::to_string(body_.size()));
	return *this;
}

boost::system::error_code Response::SendTo(tcp::socket& socket) {
	std::stringstream response;
	response << "HTTP/1.1 " << code_ << " " << Code() << "\r\n";

	for (const auto& [header_name, value] : headers_) {
		response << header_name << ": " << value << "\r\n";
	}

	response << "\r\n" << body_;

	auto s = response.str();

	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(s), error);
	return error;
}

void do_session(tcp::socket& socket, Suggestion& SG) {
	Data_Handler DH;
	beast::error_code ec;
	beast::http::request<beast::http::string_body> req;
	beast::flat_buffer buffer;
	beast::http::read(socket, buffer, req, ec);
	json json_response;
	if (req.body.size() != 0) {
		DH.file_save(req.body);
		DH.body_parser();
		json_response = SG.suggest(DH.data_body);
		std::cout << json_response.dump(4) << std::endl;
	}
	Response response;
	response.AddHeader("Content-Type", "application/json")
		.AddHeader("HeaderName", "120340120340132")
		.SetCode(200)
		.SetBody(json_response.dump());
	response.SendTo(socket);
}

void update_suggestion(Suggestion& sg) {
	sg.update_suggestions();
}
