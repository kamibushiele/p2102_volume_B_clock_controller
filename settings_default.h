#include "settings.h"

setting_t settings_default = {//構造体の.初期化を入れ子にするとエラー
    "ntp.nict.jp",
    "Asia/Tokyo",
    {
        "default_ssid",
        "default_password",
        false,
        {192,168,1,21},
        {192,168,1,1},
        {255,255,255,0}
    },
    {
        MODEL_NAME"_",
        "01234567",//8文字以上
        true,
        {192,168,1,1},
        {192,168,1,1},
        {255,255,255,0}
    }
};