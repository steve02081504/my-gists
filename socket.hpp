#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <string>
#include <stdexcept>
#pragma comment(lib,"ws2_32.lib")

namespace Socket_link_n{
	inline struct WSA_t{
		static inline bool inited = 0;
		void init(){
			if(inited)
				return;
			WSADATA wsaData = {0};
			if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
				throw (std::runtime_error)std::to_string(WSAGetLastError());
		}
		void uninit()noexcept{
			if(inited)
				WSACleanup();
			inited = 0;
		}
		~WSA_t(){uninit();}
	}WSA{};

	class Socket_link_t{
		SOCKET _clientSocket;
		SOCKADDR_IN _srvAddr;

		void link() {
			//连接服务器
			if(connect(_clientSocket, (SOCKADDR*)&_srvAddr, sizeof(SOCKADDR)))
				throw(std::runtime_error) "connect(_clientSocket, (SOCKADDR*)&_srvAddr, sizeof(SOCKADDR)) execute failed!";
		}
		void init_socket() {
			_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (_clientSocket == INVALID_SOCKET)
				throw (std::runtime_error)"_clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!";
		}
	public:
		static constexpr struct not_link_t {} not_link{};

		Socket_link_t(std::string addr,unsigned int port,not_link_t){
			WSA.init();
			init_socket();
			//初始化服务器端地址族变量
			
			_srvAddr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
			_srvAddr.sin_family = AF_INET;
			_srvAddr.sin_port = htons((u_short)port);
		}
		Socket_link_t(std::string addr, unsigned int port) :Socket_link_t(addr, port, not_link) {
			link();
		}
		void relink(){
			closesocket(_clientSocket);
			init_socket();
			link();
		}
		Socket_link_t(SOCKET clientSocket)noexcept{
			_clientSocket = clientSocket;
			_srvAddr={};
		}
		~Socket_link_t(){
			//清理
			closesocket(_clientSocket);
		}
		void base_send(const void*data,size_t size)noexcept{
			//发送消息
			::send(_clientSocket, (const char*)data,size,0);
		}
		void base_send(const std::string& massage) noexcept {
			//发送消息
			base_send(massage.c_str(),massage.size());
		}
		std::string base_get_ret(){
			//接收消息
			std::string massage;
			char recvBuf[513];
			while(1){
				const auto a= ::recv(_clientSocket, recvBuf, 512, 0);
				if (a == EAGAIN || a == 0 || a==-1)
					break;
				else{
					recvBuf[512]=0;
					massage += std::string_view{recvBuf,(unsigned)a};
				}
			}
			return massage;
		}
	};

	struct Socket_t{
		SOCKET _serverSocket;
		SOCKADDR_IN _srvAddr;
		Socket_t(unsigned int port){
			WSA.init();
			//创建套接字
			_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (_serverSocket == INVALID_SOCKET)
				throw (std::runtime_error)"_serverSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!";
			//初始化服务器地址族变量
			_srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			_srvAddr.sin_family = AF_INET;
			_srvAddr.sin_port = htons((u_short)port);

			//绑定
			if (bind(_serverSocket, (SOCKADDR*)&_srvAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
				throw (std::runtime_error)"bind(_serverSocket, (SOCKADDR*)&_srvAddr, sizeof(SOCKADDR)) execute failed!";
		}
		~Socket_t(){
			//清理
			closesocket(_serverSocket);
		}
		Socket_link_t get_link(){
			//等待连接
			SOCKADDR_IN clientAddr;
			int len = sizeof(SOCKADDR);
			const SOCKET connSocket = accept(_serverSocket, (SOCKADDR*)&clientAddr, &len);
			if (connSocket == INVALID_SOCKET)
				throw (std::runtime_error)"accept(_serverSocket, (SOCKADDR*)&clientAddr, &len) execute failed!";
			return connSocket;
		}
		void listen(){
			//监听
			if (::listen(_serverSocket, 10) == SOCKET_ERROR)
				throw (std::runtime_error)"listen(_serverSocket, 10) execute failed!";
		}
	};
}
