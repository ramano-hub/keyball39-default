/* 
 * クリックレイヤー機能の実装ファイル
 * トラックボール検知で自動的にクリックレイヤーに切り替える機能を提供します
 */

#include "auto_click_layer.h"

// グローバル変数の実体定義
enum ball_state state = NONE;
uint16_t click_timer = 0;
int16_t mouse_movement = 0;         // マウスの動き量

// ローカル変数
static bool rgb_was_enabled = false;       // RGB有効状態の保存
static uint8_t saved_mode = 0;             // 保存されたRGBモード
static HSV saved_hsv = {0, 0, 0};          // 保存されたHSV値
static bool mouse_moved_recently = false; // 直前のマウス動作を記録するフラグ

// 初期化関数
void auto_click_layer_init(void) {
    state = NONE;
    mouse_movement = 0;
    click_timer = 0;
    
    // RGB LEDの初期状態を設定
    #ifdef RGBLIGHT_ENABLE
    rgb_was_enabled = rgblight_is_enabled();
    if (rgb_was_enabled) {
        saved_mode = rgblight_get_mode();
        saved_hsv = rgblight_get_hsv();
    }
    #endif
}

// クリックレイヤーを有効にする
void enable_click_layer(void) {
    layer_on(CLICK_LAYER);
    click_timer = timer_read();
    state = CLICKABLE;
    
    // 現在のRGB状態を保存
    #ifdef RGBLIGHT_ENABLE
    rgb_was_enabled = rgblight_is_enabled();
    if (rgb_was_enabled) {
        saved_mode = rgblight_get_mode();
        saved_hsv = rgblight_get_hsv();
    }
    
    // LEDを白色に設定
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv_noeeprom(HSV_WHITE);
    #endif
}

// クリックレイヤーを無効にする
void disable_click_layer(void) {
    state = NONE;
    layer_off(CLICK_LAYER);
    mouse_movement = 0; // マウスの動き量をリセット
    
    // LEDを元の状態に戻す
    #ifdef RGBLIGHT_ENABLE
    // 保存していた状態に戻す
    if (rgb_was_enabled) {
        rgblight_enable_noeeprom();
        rgblight_mode_noeeprom(saved_mode);
        rgblight_sethsv_noeeprom(saved_hsv.h, saved_hsv.s, saved_hsv.v);
    } else {
        rgblight_disable_noeeprom();
    }
    #endif
}

// ポインティングデバイスのタスクフック
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // マウスが動いているかどうかチェック
    if (mouse_report.x != 0 || mouse_report.y != 0) {
        // マウスが動いたときのアクションを各状態ごとに制御する
        switch (state) {
            case WAITING:
                mouse_movement += my_abs(mouse_report.x) + my_abs(mouse_report.y);
                if (mouse_movement >= MOVEMENT_THRESHOLD) {
                    mouse_movement = 0;
                    enable_click_layer();
                }
                break;
                
            case CLICKABLE:
                click_timer = timer_read();
                break;
                
            case CLICKING: case CLICKED:
                // 何もしない
                break;
                
            default:
                click_timer = timer_read();
                state = WAITING;
                mouse_movement = my_abs(mouse_report.x) + my_abs(mouse_report.y);
        }
    } else {
        // マウスが動いていないとき
        switch (state) {
            case WAITING:
                if (timer_elapsed(click_timer) > 50) {
                    mouse_movement = 0;
                    state = NONE;
                }
                break;
                
            case CLICKABLE:
                if (timer_elapsed(click_timer) > CLICKABLE_TIMEOUT) {
                    disable_click_layer();
                }
                break;
                
            case CLICKING:
                // 何もしない
                break;
                
            case CLICKED:
                // CLICKED状態からCLICKABLE状態に変更し、タイマーをリセット
                // これにより、マウス操作中にレイヤーが無効化されることを防止
                state = CLICKABLE;
                click_timer = timer_read();
                break;
                
            default:
                mouse_movement = 0;
                state = NONE;
        }
        
        // マウス動作フラグをリセット
        mouse_moved_recently = false;
    }
    
    return mouse_report;
}

// キー処理フック
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // KC_NOキーが押された場合はクリックレイヤーを無効化
    if (keycode == KC_NO && record->event.pressed && (state == CLICKABLE || state == CLICKING || state == CLICKED)) {
        disable_click_layer();
        return true;
    }
    
    // クリックレイヤーがアクティブな場合の処理
    if (state == CLICKABLE || state == CLICKING || state == CLICKED) {
        if (record->event.pressed) {
            // クリックレイヤー上のキーマップを取得
            uint16_t click_layer_keycode = keymap_key_to_keycode(CLICK_LAYER, record->event.key);
            
            // クリックレイヤー上で透過キーが設定されている場合
            if (click_layer_keycode == KC_TRNS) {
                // 透過キーの場合はクリックレイヤーを無効化
                disable_click_layer();
                return true;
            }
            
            // クリック状態に移行
            state = CLICKING;
        } else {
            // キーが離された場合
            if (state == CLICKING) {
                // クリック直後状態に移行
                state = CLICKED;
                click_timer = timer_read();
            }
        }
    }
    
    return true; // 通常のキー処理を続行
}

// Keyballの動きをマウスに変換する関数
void keyball_on_mouse_move(keyball_motion_t *m, report_mouse_t *r, bool is_left) {
    // トラックボールの動きを検出したら、動き量を記録
    if (m->x != 0 || m->y != 0) {
        // マウスが動いたことを記録
        mouse_moved_recently = true;
        
        // 状態がNONEの場合はクリックレイヤーの状態遷移を開始
        if (state == NONE) {
            state = WAITING;
            click_timer = timer_read();
            mouse_movement = my_abs(m->x) + my_abs(m->y);
        } else if (state == WAITING) {
            // 動き量を累積
            mouse_movement += my_abs(m->x) + my_abs(m->y);
            // 一定以上の動きを検知したらクリックレイヤーを有効化
            if (mouse_movement >= MOVEMENT_THRESHOLD) {
                enable_click_layer();
            }
        } else if (state == CLICKABLE || state == CLICKED) {
            // クリック可能状態またはクリック直後状態ではタイマーをリセットし、クリック可能状態にする
            state = CLICKABLE;
            click_timer = timer_read();
        }
    }
}