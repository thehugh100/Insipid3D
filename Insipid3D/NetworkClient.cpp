#include "NetworkClient.h"
#include "engine.h"

//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <sstream>

#include <nlohmann/json.hpp>

//namespace beast = boost::beast;         // from <boost/beast.hpp>
//namespace http = beast::http;           // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;            // from <boost/asio.hpp>
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//
NetworkClient::NetworkClient(Engine* engine)
	:engine(engine)
{

}

////------------------------------------------------------------------------------
//
//// Report a failure
//void
//fail(beast::error_code ec, char const* what)
//{
//	std::cerr << what << ": " << ec.message() << " : " << ec.value() << "\n";
//}
//
//// Sends a WebSocket message and prints the response
//class session : public std::enable_shared_from_this<session>
//{
//	tcp::resolver resolver_;
//	websocket::stream<beast::tcp_stream> ws_;
//	beast::flat_buffer buffer_;
//	std::string host_;
//	std::string text_;
//	Engine* engine;
//
//public:
//	// Resolver and socket require an io_context
//	explicit session(net::io_context& ioc)
//		: resolver_(net::make_strand(ioc))
//		, ws_(net::make_strand(ioc))
//	{
//	}
//
//	// Start the asynchronous operation
//	void run(char const* host, char const* port, char const* username, Engine* enginePtr)
//	{
//		// Save these for later
//		host_ = host;
//		text_ = username;
//		engine = enginePtr;
//		// Look up the domain name
//		resolver_.async_resolve(
//			host,
//			port,
//			beast::bind_front_handler(
//				&session::on_resolve,
//				shared_from_this()));
//	}
//
//	void on_resolve(beast::error_code ec, tcp::resolver::results_type results)
//	{
//		if (ec)
//			return fail(ec, "resolve");
//
//		// Set the timeout for the operation
//		beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(5));
//
//		// Make the connection on the IP address we get from a lookup
//		beast::get_lowest_layer(ws_).async_connect(
//			results,
//			beast::bind_front_handler(
//				&session::on_connect,
//				shared_from_this()));
//	}
//
//	void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
//	{
//		if (ec)
//		{
//			engine->console->consolePrint("Connection failed: " + ec.message());
//			return fail(ec, "connect");
//		}
//		// Turn off the timeout on the tcp_stream, because
//		// the websocket stream has its own timeout system.
//		beast::get_lowest_layer(ws_).expires_never();
//
//		// Set suggested timeout settings for the websocket
//		ws_.set_option(
//			websocket::stream_base::timeout::suggested(
//				beast::role_type::client));
//
//		// Set a decorator to change the User-Agent of the handshake
//		ws_.set_option(websocket::stream_base::decorator(
//			[](websocket::request_type & req)
//			{
//				req.set(http::field::user_agent,
//					std::string(BOOST_BEAST_VERSION_STRING) +
//					" websocket-client-async");
//			}));
//
//		// Perform the websocket handshake
//		ws_.async_handshake(host_, "/",
//			beast::bind_front_handler(
//				&session::on_handshake,
//				shared_from_this()));
//	}
//
//	void on_handshake(beast::error_code ec)
//	{
//		if (ec)
//			return fail(ec, "handshake");
//
//		// Send the message
//		ws_.async_write(
//			net::buffer(text_),
//			beast::bind_front_handler(
//				&session::on_write,
//				shared_from_this()));
//	}
//
//	void on_write(beast::error_code ec,	std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		if (ec)
//			return fail(ec, "write");
//
//		// Read a message into our buffer
//		ws_.async_read(
//			buffer_,
//			beast::bind_front_handler(
//				&session::on_read,
//				shared_from_this()));
//	}
//
//	void on_read(beast::error_code ec,	std::size_t bytes_transferred)
//	{
//		boost::ignore_unused(bytes_transferred);
//
//		if (ec == websocket::error::closed ||
//			ec == boost::system::errc::connection_reset ||
//			ec == boost::system::errc::operation_canceled)
//		{
//			engine->console->consolePrint("Connection lost with server.");
//			return;
//		}
//
//		if (ec)
//		{
//			engine->console->consolePrint("Connection Error: " + ec.message());
//			return fail(ec, "read");
//		}
//
//		//std::cout << beast::make_printable(buffer_.data()) << std::endl;
//
//		std::ostringstream s;
//		s << beast::make_printable(buffer_.data()) << std::endl;
//		buffer_.consume(buffer_.size());
//		engine->console->consolePrint(s.str());
//
//		// Close the WebSocket connection
//		/*ws_.async_close(websocket::close_code::normal,
//			beast::bind_front_handler(
//				&session::on_close,
//				shared_from_this()));*/
//
//		ws_.async_read(
//			buffer_,
//			beast::bind_front_handler(
//				&session::on_read,
//				shared_from_this()));
//	}
//
//	void on_close(beast::error_code ec)
//	{
//		if (ec)
//			return fail(ec, "close");
//	}
//};
//
//void NetworkClient::connect(std::string address)
//{
//	engine->console->consolePrint("Attempting Connection: '" + address + "'");
//
//	nlohmann::json connection;
//	connection["type"] = "connect";
//	connection["username"] = "thehugh100";
//
//	std::make_shared<session>(ioc)->run(address.c_str(), "10000", connection.dump().c_str(), engine);
//
//	ioc_thread = std::thread([&]{ ioc.run(); });
//	ioc_thread.detach();
//}
