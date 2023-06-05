요구 사항
client.c와 server.c를 수정해서 Consensus 기반의 분산시스템을 구축하세요. 이번 과제에서는 put은 사용하지 않습니다. get만 사용한다고 가정합니다.
A. 클라이언트가 get 명령어를 수행하면 해당 요청은 모든 서버에 전송됩니다.
B. 각 서버는 get 명령어를 받으면 클라이언트에 답신을 보냅니다.
C. 우리는 총 3개의 서버를 고려합니다. 이 때 1개의 서버는 key 값이 무엇이든 간에 1을 return합니다.
실제로는 그렇지 않지만, 단순화 시킨 것입니다. 즉, get함수를 쓸 필요가 없습니다.
D. 남은 2개의 서버들은 랜덤으로 1~3 사이(즉 1 or 2 or 3)의 값을 return합니다.
E. 클라이언트는 도착한 메시지를 수합해서 2개 이상의 서버가 동일한 값을 return 했는지 여부를 출력
합니다. 2개 이상의 서버가 동일한 값을 return했으면 TRUE, 아니면 FALSE를 출력합니다.
2. 자세한동작명세는본문서하단의출력예시를참고하세요.