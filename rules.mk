RGBLIGHT_ENABLE = yes

OLED_ENABLE = yes

# VIA/Remap対応の設定
VIA_ENABLE = yes
REMAP_ENABLE = yes
DYNAMIC_KEYMAP_LAYER_COUNT = 5  # レイヤー数を5に設定（クリックレイヤー含む）

# クリックレイヤー機能のソースファイルを追加
SRC += features/auto_click_layer.c