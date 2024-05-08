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

    Php::Value create()
    {
        w = webview_create(0, NULL);
        /* webview_set_title(w, "Basic Example");
        webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
        webview_set_html(w, "Thanks for using webview!"); */
        /* webview_run(w);
        webview_destroy(w); */
        return this;
    }

    Php::Value run()
    {
        webview_run(w);
        return this;
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
        view.method<&WebView::create>("create");
        view.method<&WebView::run>("run");

        extension.add(std::move(view));
        // return the extension module
        return extension;
    }
}
