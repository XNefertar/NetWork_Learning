//#pragma once
//#pragma warning(disable: 4996)
//#pragma comment(lib, "Ws2_32.lib")
//
//#include <iostream>
//#include <string>
//#include <thread>
//#include <winsock2.h>
//#include <boost/locale.hpp>
//#include <boost/locale/encoding_utf.hpp>
//
//namespace client {
//	class SocketWinClient {
//	private:
//		SOCKET _sockfd;
//		uint16_t _port;
//		std::string _ip;
//
//	public:
//		SocketWinClient(const std::string& ip = "192.168.209.135", uint16_t port = 8080, int sockfd = -1)
//			:_ip(ip),
//			_port(port),
//			_sockfd(sockfd)
//		{}
//
//		SocketWinClient(const SocketWinClient&) = delete;
//		SocketWinClient& operator=(const SocketWinClient&) = delete;
//		~SocketWinClient() = default;
//
//		void init() {
//			WSADATA wsaData;
//			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//			if (iResult != 0) {
//				std::cout << "WSAStartup failed with error: " << iResult << std::endl;
//				return;
//			}
//
//			// Create socket
//			_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//			if (_sockfd == INVALID_SOCKET) {
//				std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
//				std::cout << "socket success: " << " : " << _sockfd << std::endl;
//				WSACleanup();
//			}
//
//			//// Connect to server
//			//sockaddr_in serverAddr;
//			//memset(&serverAddr, 0, sizeof(serverAddr));
//			//serverAddr.sin_family = AF_INET;
//			//serverAddr.sin_port = htons(_port);
//			//serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
//		}
//
//		static void getMessage(void* args, std::thread& t1){
//			t1.detach();
//			SOCKET _sockfd = *(static_cast<SOCKET*>(args));
//			sockaddr_in temp;
//			int len = sizeof(temp);
//			// receive message
//			char recvbuf[1024]{};
//			size_t s = recvfrom(_sockfd, recvbuf, 1024, 0, (sockaddr*)&temp, &len);
//			if (s == SOCKET_ERROR) {
//				std::cout << "Error at recvfrom(): " << WSAGetLastError() << std::endl;
//				closesocket(_sockfd);
//				WSACleanup();
//				return;
//			}
//			// 转回utf-8格式
//			std::string response = recvbuf;
//			response = boost::locale::conv::from_utf(response, "UTF-8");
//			std::cout << "Response # " << response << std::endl;
//		}
//
//		void start() {
//			std::thread t1(&SocketWinClient::getMessage, &_sockfd, std::ref(t1));
//			sockaddr_in _serverAddr;
//			memset(&_serverAddr, 0, sizeof(_serverAddr));
//			_serverAddr.sin_family = AF_INET;
//			_serverAddr.sin_port = htons(_port);
//			_serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
//
//			if (connect(_sockfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR) {
//				std::cout << "Error at connect(): " << WSAGetLastError() << std::endl;
//				closesocket(_sockfd);
//				WSACleanup();
//				return;
//			}
//			while (1) {
//				// Send message
//				std::string sendbuf;
//				std::cout << "Please input message # ";
//				getline(std::cin, sendbuf);
//				// 转为utf-8格式
//				// 以匹配Linux端的utf-8格式
//				sendbuf = boost::locale::conv::to_utf<char>(sendbuf, "UTF-8");
//
//				sendto(_sockfd, sendbuf.c_str(), sendbuf.length(), 0, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
//
//
//			}
//		}
//
//	};
//}


#pragma once
#pragma warning(disable: 4996)
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <boost/locale.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <chrono>

namespace client {
	class SocketWinClient {
	private:
		SOCKET _sockfd;
		uint16_t _port;
		std::string _ip;
		std::thread _thread; // 将线程对象作为类的成员变量

	public:
		SocketWinClient(const std::string& ip = "192.168.209.135", uint16_t port = 8080, int sockfd = -1)
			:_ip(ip),
			_port(port),
			_sockfd(sockfd)
		{}

		SocketWinClient(const SocketWinClient&) = delete;
		SocketWinClient& operator=(const SocketWinClient&) = delete;
		~SocketWinClient() {
			if (_thread.joinable()) {
				_thread.join(); // 确保线程在析构时被正确地结束
			}
		}

		void init() {
			WSADATA wsaData;
			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				std::cout << "WSAStartup failed with error: " << iResult << std::endl;
				return;
			}

			// Create socket
			_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (_sockfd == INVALID_SOCKET) {
				std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
				std::cout << "socket success: " << " : " << _sockfd << std::endl;
				WSACleanup();
			}
		}

		static void getMessage(SocketWinClient* client) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//client->_thread.detach();
			SOCKET _sockfd = client->_sockfd;
			while (1) {
				sockaddr_in temp;
				int len = sizeof(temp);
				// receive message
				char recvbuf[1024]{};
				size_t s = recvfrom(_sockfd, recvbuf, 1024, 0, (sockaddr*)&temp, &len);
				if (s == SOCKET_ERROR) {
					std::cout << "Error at recvfrom(): " << WSAGetLastError() << std::endl;
					closesocket(_sockfd);
					WSACleanup();
					return;
				}
				// 转回GBK格式
				std::string response = recvbuf;
				response = boost::locale::conv::from_utf(response, "GBK");
				std::cout << "Response # " << response << std::endl;
			}
		}

		void start() {
			_thread = std::thread(&SocketWinClient::getMessage, this); // 启动线程
			sockaddr_in _serverAddr;
			memset(&_serverAddr, 0, sizeof(_serverAddr));
			_serverAddr.sin_family = AF_INET;
			_serverAddr.sin_port = htons(_port);
			_serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());

			if (connect(_sockfd, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR) {
				std::cout << "Error at connect(): " << WSAGetLastError() << std::endl;
				closesocket(_sockfd);
				WSACleanup();
				return;
			}
			while (1) {
				// Send message
				std::string sendbuf;
				std::cout << "Please input message # ";
				getline(std::cin, sendbuf);
				// 转为utf-8格式
				// 以匹配Linux端的utf-8格式
				sendbuf = boost::locale::conv::to_utf<char>(sendbuf, "GBK");
				sendto(_sockfd, sendbuf.c_str(), sendbuf.length(), 0, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
			}
		}
	};
}