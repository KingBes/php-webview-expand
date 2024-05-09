/**
 *  extension.cpp
 *  @author Jasper van Eck<jasper.vaneck@copernica.com>
 *
 *  An example file to show the working of an extension.
 */
// library include
#include <phpcpp.h>
#include "webview.h"
#include <stddef.h>

// 类定义开始之前，先定义全局的回调删除器，用于在不再需要时释放PHP::Value的内存
class CallbackDeleter
{
public:
    void operator()(Php::Value *ptr)
    {
        delete ptr;
    }
};

// 使用智能指针管理callback的内存，确保适当时机自动释放
typedef std::unique_ptr<Php::Value, CallbackDeleter> CallbackPtr;

// 适配器函数，用于桥接C++与PHP回调
void phpCallbackAdapter(const char *seq, const char *req, void *arg)
{
    CallbackPtr &callbackPtr = *(CallbackPtr *)arg;
    Php::Value &callback = *callbackPtr;

    // 将C字符串转换为PHP::String并调用PHP回调
    callback(std::string(seq), std::string(req));
}

class WebView : public Php::Base
{
private:
    /* data */
    webview_t w;

public:
    /**
     *  c++构造函数
     */
    WebView() = default;

    /**
     *  c++析构函数
     */
    virtual ~WebView() = default;

    // 创建
    Php::Value create(Php::Parameters &params)
    {
        w = webview_create(params[0], NULL);
        return this;
    }

    // 初始化注入js,代码在window.onload之前执行
    Php::Value init(Php::Parameters &params)
    {
        Php::Value value = params[0];
        webview_init(w, value);
        return this;
    }

    // 计算任意JavaScript代码。求值也是异步进行的
    // 表达式的结果被忽略。如果您愿意，可以使用RPC绑定
    // 接收关于评估结果的通知。
    Php::Value eval(Php::Parameters &params)
    {
        Php::Value value = params[0];
        webview_eval(w, value);
        return this;
    }

    // 设置标题
    Php::Value title(Php::Parameters &params)
    {
        Php::Value value = params[0];
        webview_set_title(w, value);
        return this;
    }

    // 设置大小
    Php::Value size(Php::Parameters &params)
    {
        int hint = (int)params[2] <= 3 ? (int)params[2] : 0;
        webview_set_size(w, (int)params[0], (int)params[1], hint);
        return this;
    }

    // 设置html字符串
    Php::Value html(Php::Parameters &params)
    {
        std::string htmlContent = params[0].stringValue();
        webview_set_html(w, htmlContent);
        return this;
    }

    // 设置url
    Php::Value navigate(Php::Parameters &params)
    {
        std::string urlContent = params[0].stringValue();
        webview_navigate(w, urlContent);
        return this;
    }

    // 绑定js全局函数
    Php::Value bind(Php::Parameters &params)
    {
        Php::Value value = params[0];
        Php::Value callback = params[1]; // 第二个参数是php函数 function($seq, json_decode($req))
        // 使用智能指针管理callback的内存
        CallbackPtr callbackPtr(new Php::Value(callback));
        // 确保 callback 是可调用的
        if (!callback.isCallable())
        {
            throw Php::Exception("Second parameter must be a callable.");
        }

        webview_bind(w, value, phpCallbackAdapter, (void *)&callbackPtr);
        return this;
    }

    // 解除绑定js
    Php::Value unbind(Php::Parameters &params)
    {
        Php::Value value = params[0];
        webview_unbind(w, value);
        return this;
    }

    // 返回
    Php::Value ret(Php::Parameters &params)
    {
        Php::Value value = params[0];
        Php::Value res = params[1];
        webview_return(w, value, 0, res);
        return this;
    }

    // 运行
    Php::Value run()
    {
        webview_run(w);
        return this;
    }

    // 停止主循环
    Php::Value terminate()
    {
        webview_terminate(w);
        return this;
    }

    // 关闭
    void destroy()
    {
        webview_destroy(w);
    }
};

// Symbols are exported according to the "C" language
extern "C"
{
    // export the "get_module" function that will be called by the Zend engine
    PHPCPP_EXPORT void *get_module()
    {
        // create extension
        static Php::Extension extension("webview", "1.0");

        Php::Class<WebView> view("WebView");

        view.method<&WebView::create>("create", {Php::ByVal("debug", Php::Type::Bool)});
        view.method<&WebView::init>("init", {Php::ByVal("js", Php::Type::String)});
        view.method<&WebView::eval>("eval", {Php::ByVal("js", Php::Type::String)});
        view.method<&WebView::title>("title", {Php::ByVal("title", Php::Type::String)});
        view.method<&WebView::size>("size", {
                                                Php::ByVal("width", Php::Type::Numeric),
                                                Php::ByVal("heigh", Php::Type::Numeric),
                                                Php::ByVal("hint", Php::Type::Numeric),
                                            });
        view.method<&WebView::html>("html", {Php::ByVal("html", Php::Type::String)});
        view.method<&WebView::navigate>("navigate", {Php::ByVal("url", Php::Type::String)});
        view.method<&WebView::bind>("bind", {
                                                Php::ByVal("name", Php::Type::String),
                                                Php::ByVal("callback"),
                                            });
        view.method<&WebView::unbind>("unbind", {Php::ByVal("name", Php::Type::String)});
        view.method<&WebView::ret>("ret", {
                                              Php::ByVal("seq", Php::Type::String),
                                              Php::ByVal("value"),
                                          });
        view.method<&WebView::run>("run");
        view.method<&WebView::destroy>("destroy");
        view.method<&WebView::terminate>("terminate");

        extension.add(std::move(view));
        // return the extension module
        return extension;
    }
}
