1. server.c를 적절히 수정해서 leader와 follower 역할을 수행할 수 있는 서버프로그램을 작성하세요.
- 실행시 인자를 입력해서 leader와 follower 역할을 구분할 수 있습니다. 1이면 leader, 0이면 follower
- Leader는 get 명령어를 받으면 해당하는 값을 client에게 reply로 return합니다
- Leader는 put 명령어를 받으면 다른 follower들에게 이를 전파합니다.
- Follower는 leader로부터 put 명령어를 받으면 값을 갱신 후 leader에게 ack을 회신합니다.
- Leader는 모든 Follower들로부터 ack을 회신하였다면, client에게 reply를 보내 commit합니다.

2. Key-value store 동작은 server.c에 미리 구현된 get과 put 함수를 쓰면 됩니다.
