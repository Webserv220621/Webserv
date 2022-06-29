class Webserv {

	public:
		int parseDefaultConfig() {
			return parseConfig("default.conf");
		};

		int parseConfig(std::string where);
		// config 파일의 경로를 인자로 받아서 그 내용을 Server 객체에 저장
		// 파싱 실패하면 -1 리턴

		int run() {
			// kqueue 초기화    ==> 실패하면 에러메시지 출력, -1 리턴

			// server 갯수만큼 돌면서
				// fd = server.run();    // 서버소켓 생성, bind, listen
				// kq에 서버소켓 ADD
			// ==> 실패하면 에러메시지 출력, -1 리턴


			/* 서버-클라간 커넥션들 저장할 Connections 변수 선언
				fd(or idx): 클라소켓
				Conn: Request, Response 객체를 멤버로 갖는 객체
			*/

			while (1) {
				// kq에 변화가 있는지 확인
				// ev_cnt = kevent(...., timeout);
				// ev_cnt 만큼 돌면서
					// 이벤트가 서버소켓의 READ? (커넥션 요청)
							// accept 하고 (client, READ)를 kq에 추가
							// Connection 객체 생성 ( fd, server_fd, Request, Response)

					// 이벤트가 클라소켓의 READ? (클라가 뭔가를 보냈다)
							//connection.Request에 읽은 정보를 append
							//request 파싱
								//request 수신 완료?
									// kq에서 (fd, READ 삭제)
									// connection.prosess()   ==> Response 생성
									// kq에 (fd, WRITE) 추가

					// 이벤트가 클라소켓의 WRITE? (클라가 받을 준비가 됐다)
							// 클라가 받을 수 있을 만큼 잘라서 connection.response.data 를 send()
							// 다 보냈으면 close connection
							// 덜 보냈으면 어디까지 보냈는지 check하고 다시 대기

				// connections 순회하면서 timeout 체크
			}
		}
};

