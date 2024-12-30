#include "./factory_rf.h"
#include "./../factory_app.h"

// 加载模块
#include "./link/rf_debug.cc"     // RF模块调试
#include "./link/rf_info.cc"      // RF模块信息获取
#include "./link/rf_pair.cc"      // RF模块配对
#include "./link/rf_reconnect.cc" // RF模块重连
#include "./link/rf_remap.cc"     // RF模块通道映射
#include "./update/rf_update.cc"  // RF模块更新

void factory_rf_init(void)
{
    rf_debug_init();     // RF模块调试
    rf_info_init();      // RF模块信息获取
    rf_pair_init();      // RF模块配对
    rf_remap_init();     // RF模块通道映射
    rf_update_init();    // RF模块更新
    rf_reconnect_init(); // RF模块重连
}
