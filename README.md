ChattingRoom
============

####基于UDP,C socket和linux的聊天程序

####使用说明

1. setup.c和UDPClient.c编译成客户端，setup.c和UDPServer.c编译成服务器端；
2. 先运行服务器程序(没有参数），再运行客户端程序（参数一个，为用户名）；
3. 开启多个客户端，客户端之间进行通信；
4. “@username 消息”可以将本消息指定发送给该用户，默认群发当前在线的所有客户端
5. 目前只在本地测试通过，真实网络环境还没有完成，敬请期待。

