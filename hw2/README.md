1. 만약 명령어가 get이라면:
- Key-value store를 key를 가지고 lookup(참조)합니다.
- 만약 key가 존재한다면, 해당 key에 대한 value를 클라이언트에게 보냅니다.
- 만약 key가 존재하지 않는다면, “Key not found” 메시지를 클라이언트에게 보냅니다.

2. 만약 명령어가 put이라면:
- Key-value store에 해당 key와 value를 추가/수정 합니다.
- 클라이언트에게 “OK”를 회신합니다.

3. 만약 명령어가 get 혹은 put이 아니라면:
- 클라이언트에게 “Invalid command”를 회신합니다.

4. Get 명령어의 경우 get x의 형태가 아니라면 Invalid command를 클라이언트에게 회신합니다. 예를 들어 그 냥 get만 보냈다던가 get 3 3처럼 put의 형태로 보냈다던가.. 무조건 get x만 허용합니다

5. Put 명령어의 경우 put x y의형태가 아니라면 Invalid command를 클라이언트에게 회신합니다. 예를 들어 그 냥 put만 보내거나 put x만 보냈거나, put x y z를 보내면 Invalid command를 회신합니다.
