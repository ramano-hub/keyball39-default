/* 
 * クリックレイヤー機能の実装ファイル
 * トラックボール検知で自動的にクリックレイヤーに切り替える機能を提供します
 */

#pragma once

#include "quantum.h"
#include "../../../lib/keyball/keyball.h"
#include "../utils/functions.h"

// クリックレイヤーの状態定義
enum ball_state {
  NONE = 0,    // 通常状態
  WAITING,     // マウスレイヤー有効化待ち
  CLICKABLE,   // クリック可能状態
  CLICKING,    // クリック中
  CLICKED      // クリック直後
};

// クリックレイヤーの設定
#define CLICK_LAYER 4                    // クリックレイヤーのレイヤー番号
#define CLICKABLE_TIMEOUT 1200           // クリック可能状態の維持時間（ミリ秒）
#define CLICKED_TIMEOUT 800             // クリック直後状態の維持時間（ミリ秒）
#define MOVEMENT_THRESHOLD 0             // クリックレイヤー有効化のための動き閾値

// グローバル変数
extern enum ball_state state;            // 現在の状態
extern uint16_t click_timer;             // タイマー
extern int16_t mouse_movement;           // マウスの動き量

// 関数プロトタイプ
void auto_click_layer_init(void);        // クリックレイヤーの初期化
void enable_click_layer(void);           // クリックレイヤーを有効にする
void disable_click_layer(void);          // クリックレイヤーを無効にする
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report);  // ポインティングデバイスのタスクフック
bool process_record_user(uint16_t keycode, keyrecord_t *record);        // キー処理フック