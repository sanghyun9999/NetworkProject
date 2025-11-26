//#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//
//#define _WIN32_WINNT 0x0601
//
//#include <winSock2.h>
//#include <ws2tcpip.h>
//
//#include <tchar.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <windows.h>
//
//#pragma comment(lib, "ws2_32")
//
//#define SERVERPORT 9000
//#define BUFSIZE 2048
//
//// 소켓 함수 오류 출력 후 종료
//void err_quit(const char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(char*)&lpMsgBuf, 0, NULL);
//	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
//	LocalFree(lpMsgBuf);
//	exit(1);
//}
//
//// 소켓 함수 오류 출력
//void err_display(const char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(char*)&lpMsgBuf, 0, NULL);
//	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}
//
//// 소켓 정보 저장을 위한 구조체와 변수
//struct SOCKETINFO
//{
//	SOCKET sock;
//	char buf[BUFSIZE + 1];
//	int recvbytes;
//	int sendbytes;
//};
//int nTotalSockets = 0;
//SOCKETINFO* SocketInfoArray[FD_SETSIZE];
//
//// 소켓 정보 관리 함수
//bool AddSocketInfo(SOCKET sock);
//void RemoveSocketInfo(int nIndex);
//
//int main(int argc, char* argv[]) {
//
//	int retval;
//	int opt = 0;
//
//	// 윈속 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//		return 1;
//
//	// IPv6 TCP 소켓 생성
//	SOCKET tcp_sock = socket(AF_INET6, SOCK_STREAM, 0);
//	// IPv6 UDP 소켓 생성
//	SOCKET udp_sock = socket(AF_INET6, SOCK_DGRAM, 0);
//	if (tcp_sock == INVALID_SOCKET || udp_sock == INVALID_SOCKET) err_quit("socket()");
//
//	// 듀얼 스택 설정
//	if (setsockopt(tcp_sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
//		err_quit("setsockopt tcp");
//	if (setsockopt(udp_sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
//		err_quit("setsockopt udp");
//
//	// 구조체
//	struct sockaddr_in6 serveraddr;
//	memset(&serveraddr, 0, sizeof(serveraddr));
//	serveraddr.sin6_family = AF_INET6;
//	serveraddr.sin6_port = htons(SERVERPORT);
//	serveraddr.sin6_addr = in6addr_any;
//
//	// tcp bind()
//	retval = bind(tcp_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit("bind() - TCP");
//
//	// listen()
//	retval = listen(tcp_sock, SOMAXCONN);
//	if (retval == SOCKET_ERROR) err_quit("listen()");
//
//	// udp bind()
//	retval = bind(udp_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit("bind() - UDP");
//
//	fd_set reads, copy_reads;
//	TIMEVAL timeout;
//
//	while (1) {
//		// 소켓 셋 초기화
//		FD_ZERO(&reads);
//		FD_SET(tcp_sock, &reads);
//		FD_SET(udp_sock, &reads);
//		for (int i = 0; i < nTotalSockets; i++) {
//			FD_SET(SocketInfoArray[i]->sock, &reads);
//		}
//
//		// 타임아웃 설정
//		timeout.tv_sec = 0;
//		timeout.tv_usec = 100000;
//
//		copy_reads = reads;
//		retval = select(0, &copy_reads, 0, 0, &timeout);
//
//		if (retval == SOCKET_ERROR) {
//			err_display("select()");
//			break;
//		}
//		if (retval == 0) continue;
//
//		// [UDP 처리 구역
//		if (FD_ISSET(udp_sock, &copy_reads)) {
//			// 데이터 통신에 사용할 변수
//			struct sockaddr_in6 clientaddr;
//			int addrlen;
//			char buf[BUFSIZE + 1];
//
//			// 데이터 받기
//			addrlen = sizeof(clientaddr);
//
//			retval = recvfrom(udp_sock, buf, BUFSIZE, 0,
//				(struct sockaddr*)&clientaddr, &addrlen);
//
//			if (retval == SOCKET_ERROR) {
//				err_display("recvfrom()");
//			}
//
//			else if (retval > 0) {
//				// 데이터 보내기
//				retval = sendto(udp_sock, buf, retval, 0,
//					(struct sockaddr*)&clientaddr, sizeof(clientaddr));
//
//				if (retval == SOCKET_ERROR) {
//					err_display("sendto()");
//				}
//			}
//		}
//
//		// [TCP 연결 수락 구역] 
//		if (FD_ISSET(tcp_sock, &copy_reads)) {
//			// 데이터 통신에 사용할 변수
//			SOCKET client_sock;
//			struct sockaddr_in6 clientaddr;
//			int addrlen;
//
//			// accept()
//			addrlen = sizeof(clientaddr);
//			client_sock = accept(tcp_sock, (struct sockaddr*)&clientaddr, &addrlen);
//			if (client_sock == INVALID_SOCKET) {
//				err_display("accept()");
//			}
//			else {
//				char addr[INET6_ADDRSTRLEN];
//				// 접속한 클라이언트의 정보 출력
//				inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));
//				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
//					addr, ntohs(clientaddr.sin6_port));
//
//				// 접속한 소켓을 관리 리스트에 추가.
//				if (AddSocketInfo(client_sock) == false) {
//					closesocket(client_sock);
//				}
//			}
//		}
//
//
//		// 이미 연결된 TCP 클라이언트들이 보낸 데이터가 있는지 확인
//		for (int i = 0; i < nTotalSockets; i++) {
//			SOCKETINFO* ptr = SocketInfoArray[i];
//
//			// [TCP 데이터 수신 구역]
//			if (FD_ISSET(ptr->sock, &copy_reads)) {
//				// 데이터 받기
//				retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
//
//				if (retval == SOCKET_ERROR) {
//					err_display("recv()");
//					RemoveSocketInfo(i);
//					i--;
//				}
//				else if (retval == 0) {
//					// 정상 종료
//					RemoveSocketInfo(i);
//					i--;
//				}
//				else {
//					// 에코(Echo) 전송
//					send(ptr->sock, ptr->buf, retval, 0);
//				}
//			}
//		}
//	}
//
//	// 윈속 종료
//	WSACleanup();
//	return 0;
//}
//
//// 소켓 정보 추가
//bool AddSocketInfo(SOCKET sock)
//{
//	if (nTotalSockets >= FD_SETSIZE) {
//		printf("[오류] 소켓 정보를 추가할 수 없습니다.\n");
//		return false;
//	}
//	SOCKETINFO* ptr = new SOCKETINFO;
//	if (ptr == NULL) {
//		printf("[오류] 메모리가 부족합니다.\n");
//		return false;
//	}
//	ptr->sock = sock;
//	ptr->recvbytes = 0;
//	ptr->sendbytes = 0;
//	SocketInfoArray[nTotalSockets++] = ptr;
//	return true;
//}
//
//// 소켓 정보 삭제
//void RemoveSocketInfo(int nIndex)
//{
//	SOCKETINFO* ptr = SocketInfoArray[nIndex];
//
//	// 클라이언트 정보 얻기
//	struct sockaddr_in6 clientaddr;
//	int addrlen = sizeof(clientaddr);
//	getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);
//
//	// 클라이언트 정보 출력
//	char addr[INET6_ADDRSTRLEN];
//	inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));
//	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
//		addr, ntohs(clientaddr.sin6_port));
//
//	// 소켓 닫기
//	closesocket(ptr->sock);
//	delete ptr;
//
//	if (nIndex != (nTotalSockets - 1))
//		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
//	--nTotalSockets;
//}