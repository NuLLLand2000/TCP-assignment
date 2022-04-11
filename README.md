# TCP-assignment
编写一个接受文件名和 <IP|DNS>:PORT 组合以及文件名作为参数的客户端应用程序。

```Bash
./client 127.0.0.1:8888 file_name
```

应用程序应该**打开文件**，连接到 <IP|DNS>:PORT，发送一个 HEADER，然后是文件的内容。 
HEADER 是一个简单的字符串，构造为：“<filename>[space]<md5hash>\n”。这里 '[space]' 表示一个空格（十进制值 32，在 ASCII 表中），而 '\n' 是换行符 (0x0A)。 <filename> 只是正在发送的文件的名称，而 <md5hash> 是从文件内容中获得的 MD5 哈希。一旦发送了 HEADER，客户端应该等待服务器响应“OK\n”。如果服务器没有回复“OK\n”，客户端应该重新发送 HEADER，并再次读取。最多应进行 5 次尝试，每次尝试 3 秒超时，之后客户端应放弃并断开连接。收到“OK”后，应传输文件内容。传输时，客户端应统计成功传输的字节数。一旦最后一个字节被传输，它应该关闭它的连接。然后它应该打印传输的字节数和传输花费的时间，以微秒（us）（10^-6）为单位。请注意，文件名的最大长度为 256 字节，而 MD5_DIGEST_LENGHT 为 16 字节。

### 服务器端

编写一个相应的服务器，它监听一个端口（在运行时从命令行）。当服务器接收数据时，它应该计算文件数据的 MD5Hash。当服务器收到最后一条消息时，它将向 STDOUT 写入格式为：

连接时间（unix时间）|客户端 IP：端口（DNS 名称，如果可用，否则不适用）|文件名 |传输大小（字节） |传输速率 (bps) |持续时间（秒）| 【哈希比较】

如果接收和计算的 Hash 值相同，则此处 [Hash 比较] 应为“哈希匹配”，并且“哈希不匹配”后跟打印接收的哈希和计算的哈希，请参见下面的示例。

**服务器应该能够同时处理多个客户端**，如果你通过线程解决它，fork 或 select 取决于你。

一旦服务器接受了客户端，服务器在从客户端读取数据时应该有 3 秒的超时时间（等待 HEADER）。如果客户端没有发送 HEADER（带有换行符的字符串），服务器应该发送“ERROR\n”，并在放弃该客户端并关闭套接字之前尝试五次。

请注意，如果一个客户端发生故障或终止，则不应影响服务器或与服务器通信的其他客户端的操作。服务器可能终止的唯一情况是您无法 fork() 或您无法创建新线程。

# 食用说明
**确保你的环境是linux并且已经安装openssl**
没有的话请安装依赖
```bash
sudo apt-get install libssl-dev
```

1. 编译文件
```bash
gcc receivemetcp.c -o receivemetcp -lcrypto -lssl
gcc sendmetcp.c -o sendmetcp -lcrypto -lssl
```

2. 先运行服务器
```bash
./receivemetcp <your_port>
```
3. 另开terminal运行客户端
```bash
./sendmetcp 127.0.0.1:<your_port> file_name
```

4. 在receivemetcp.c的256行修改retime的值来执行两种超时的情况。