#include <iostream>
#include "../Config.hpp"

/* 테스트 컴파일을 위해 필요한 파일들: myself, ConfigWebserv.cpp, ConfigServer.cpp, util.cpp */
int main(int ac, char **av) {
	if (ac != 3) {
		std::cout << "Usage: tester config_file uri_path" << std::endl;
		return 1;
	}

	Webserv webserv;
	webserv.parsingWebserv(av[1]);

    std::string uri = av[2];
    std::cout << "요청:" << uri << "\n";
    Server s = webserv.getServerList()[0];
    const std::map<std::string, Location>& locations = s.getLocations();
    std::map<std::string, Location>::const_iterator it = locations.begin();
    for (; it != locations.end(); ++it) {
        std::cout << "탐색대상:" << it->first << std::endl;
    }
    std::cout << "\n\n";

	size_t match_chars_cnt = 0;
	std::map<std::string, Location>::const_iterator it_best_match;
	/* map 전체를 순회하면서 prefix(맵의 key)를 uri의 시작부분에서 검색.
	 * 매칭되면 매칭후보로 놓고 prefix길이를 기억.
	 * 새 매칭후보와 이전 매칭후보의 prefix 길이 비교
	 * prefix가 *로 시작하고, 매칭되면 탐색종료
	 * 전체 순회를 마친 다음 최종 후보를 리턴 */
	for (it = locations.begin(); it != locations.end(); ++it) {
		const std::string& prefix = it->first;
		std::cout << "checking " << prefix << std::endl;
		if (prefix[0] != '*') {
			size_t pos = uri.find(prefix);
			if (pos != 0)
				continue;
			if (match_chars_cnt < prefix.length()) {
				match_chars_cnt = prefix.length();
				it_best_match = it;
			}
		}
		else {
			size_t pos = uri.find(prefix.substr(1));
			if (pos == std::string::npos)
				continue;
			if (pos + prefix.length() - 1 == uri.length()) {
				it_best_match = it;
				break;
			}
		}
	}
    std::cout << "\n\nbest match: " << it_best_match->first << std::endl;

{
	/* map의 key가 각 location블록의 prefix를 갖고 있다.
	 * uri가 prefix로 시작하는지 여부를 검색.
	 * map의 특성상 이미 사전순정렬 되어 있으므로
	 * 뒤에서부터 검색하면 자동으로 prefix길이가 가장 긴 녀석과 매칭된다.
	 * ex. 요청uri가 /upload/abc이고,
	 *   location /up
	 *   location /upload
	 * 가 존재한다면, /up 대신 /upload를 선택
	 *
	 * 단 prefix가 '*'로 시작하는 녀석과도 매칭된다면, 그 쪽의 우선순위가 더 높다. */
	const Server::locations_map_type& locations = s.getLocations();
	Server::locations_map_type::const_reverse_iterator rit;
	Server::locations_map_type::const_reverse_iterator rit_best_match;
	bool prefix_found = false;
	for (rit = locations.rbegin(); rit != locations.rend(); ++rit) {
		const std::string& prefix = rit->first;
		std::cout << "checking " << prefix << std::endl;
		if (prefix[0] != '*') {
			if (prefix_found)
				continue;
			size_t pos = uri.find(prefix);
			if (pos != 0)
				continue;
			rit_best_match = rit;
			prefix_found = true;
		}
		else {
			size_t pos = uri.find(prefix.substr(1));
			if (pos == std::string::npos)
				continue;
			if (pos + prefix.length() - 1 == uri.length()) {
				rit_best_match = rit;
				break;
			}
		}
	}
    std::cout << "\n\nbest match: " << rit_best_match->first << std::endl;
}


	return 0;
}
