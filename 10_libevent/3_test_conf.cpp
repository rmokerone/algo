#include <event2/event.h>
#include <iostream>

using namespace std;

int main()
{
    std::cout << "test configure!\n" ;

    // 配置上下文
    event_config *conf = event_config_new();

    const char **methods = event_get_supported_methods();

    cout << "event_get_supported_methods: " << endl;
    for (int i = 0; methods[i] != NULL; i ++){
        cout << methods[i] << endl;
    }


    // 设置特征 
    // event_config_require_features(conf, EV_FEATURE_FDS);

    // 设置网络模型 使用select
    // event_config_avoid_method(conf , "epoll");
    // event_config_avoid_method(conf , "poll");

    

    event_base *base =  event_base_new_with_config(conf);

    if (!base){
        cerr << "event_base_new_with_config failed" << endl;
        base = event_base_new();
        if (!base) {
            cerr << "reconfig not effect" << endl;
        }
    }else {
        cout << "event_base_new_with_config success" << endl;
        // 获取当前网络模型
        cout << "current mothod " << event_base_get_method(base) << endl;

        // 判断特征是否有效
        int f = event_base_get_features(base);
        if (f & EV_FEATURE_ET){
            cout << "EV_FEATURE_ET events are supported" << endl;
        }else{
            cout << "EV_FEATURE_ET events are not supported" << endl;
        }
        if (f & EV_FEATURE_O1){
            cout << "EV_FEATURE_O1 events are supported" << endl;
        }else{
            cout << "EV_FEATURE_O1 events are not supported" << endl;
        }
        if (f & EV_FEATURE_FDS){
            cout << "EV_FEATURE_FDS events are supported" << endl;
        }else{
            cout << "EV_FEATURE_FDS events are not supported" << endl;
        }
        if (f & EV_FEATURE_EARLY_CLOSE){
            cout << "EV_FEATURE_EARLY_CLOSE events are supported" << endl;
        }else{
            cout << "EV_FEATURE_EARLY_CLOSE events are not supported" << endl;
        }
    }

    event_config_free(conf);
    
    return 0;
}