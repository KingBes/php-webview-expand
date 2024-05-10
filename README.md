# php-webview-expand

[下载](https://github.com/KingBes/php-webview-expand/releases)

配置 `php.ini` 文件

```txt
extension=webview.so
```

```php
$webview = new WebView();
```

```php
// 创建窗口 参数：debug = bool
$webview->create(debug:false);
// 初始化注入js,代码在window.onload之前执行
$webview->init(js:"alert('asd');");
// 设置窗口大小 参数：width = int ，height = int ，hint = int （0~3）
$webview->size(width:400,height:300,hint:0);
// 设置窗口标题     参数：title=string
$webview->title(title:"新的标题");
// 设置html内容     参数：html=string
$webview->html(html:"<a>html的内容</a>");
// 绑定交互的操作    参数：name=string ，闭包函数：$req 是接收到的参数,$seq 是触发次数
$webview->bind(name:"bindName",function($seq, $req){
    return ["返回内容","返回数组"];
});
// 解除绑定         参数：name=你绑定过的name名
$webview->unbind(name:"bindName");
// 设置窗口url内容  参数：url=string
$webview->navigate(url:"http://www.baidu.com");
// 运行
$webview->run();
// 销毁
$webview->destroy();
```