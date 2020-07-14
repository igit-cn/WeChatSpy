# PC微信Hook:
## 使用教程:
[https://github.com/MaoningGuan/PyWeChat](https://github.com/MaoningGuan/PyWeChat)
## 更新日志
## 1.0.0.7:
* 增加self类型说明
* 增加msg_type类型说明
* 增加防止Socket通信黏包的分隔符
## 1.0.0.6:
* 增加获取用户个人信息的功能。
## 1.0.0.5:
* 完善代码逻辑。
## 1.0.0.4：
* 增加支持发送中文文本消息的功能。
> 原因分析：  
python的data = json.dumps(data)函数会把data里面的中文以ACSII进行编码，如：  
"python server收到" --> "python server\u6536\u5230"  
导致python的socket_client.send(data.encode(encoding="utf8"))没有把中文转成对应的utf-8编码，  
而是把"python server\u6536\u5230"当成一般的英文字符串进行编码。  
最后的data为：b"python server\u6536\u5230"  
解决办法：  
修改python代码为：data = json.dumps(data, ensure_ascii=False)  
最后的data为：b"python server\xe6\x94\xb6\xe5\x88\xb0"  

## 1.0.0.3：
* 增加Python程序可以向微信发送英文文本信息的功能，暂不支持发送中文。
## 1.0.0.2：
* 增加server和client之间的心跳验证功能。
## 1.0.0.1：
* 实现了Hook微信收到的文本消息。
