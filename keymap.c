/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "features/auto_click_layer.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    KC_Q     , KC_L     , KC_U     , KC_MINS  , KC_DOT   ,                            KC_F     , KC_W     , KC_R     , KC_V     , KC_P     ,
    KC_E     , LALT_T(KC_I),LCTL_T(KC_A),LSFT_T(KC_O),    KC_COMM  ,                            KC_K     , RSFT_T(KC_T),RCTL_T(KC_N),RALT_T(KC_S),    KC_H     ,
    KC_Z     , KC_X     , KC_C     , KC_V     , KC_SCLN  ,                            KC_G     , KC_D     , KC_M     , KC_J     , KC_B     ,
    KC_A     , KC_LNG2  , LT(2,KC_BSPC), LT(1,KC_SPC), LT(3,KC_LNG1), KC_BSPC,    KC_SPC  , LT(2,KC_TAB), KC_SLSH  , KC_RALT  , KC_RGUI  , KC_RSFT
  ),
  /* 元のキーマップ（参考）
  [0] = LAYOUT_universal(
    KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                            KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     ,
    KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                            KC_H     , KC_J     , KC_K     , KC_L     , KC_MINS  ,
    KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                            KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  ,
    KC_LCTL  , KC_LGUI  , KC_LALT  ,LSFT_T(KC_LNG2),LT(1,KC_SPC),LT(3,KC_LNG1),KC_BSPC,LT(2,KC_ENT),LSFT_T(KC_LNG2),KC_RALT,KC_RGUI, KC_RSFT
  ),
  */

  [1] = LAYOUT_universal(
    KC_TAB   , KC_F3    , KC_PSCR  , KC_PSCR  , KC_MINS  ,                            KC_PIPE  , KC_UNDS  , KC_UP    , KC_UNDS  , KC_BSPC  ,
    S(KC_3)  , KC_F7    , KC_AT    , KC_HASH  , S(KC_SCLN),                           S(KC_9)  , KC_LEFT  , KC_DOWN  , KC_RGHT  , KC_QUOT  ,
    KC_SLSH  , KC_1     , KC_2     , KC_3     , S(KC_MINS),                           S(KC_0)  , KC_LEFT  , KC_DOWN  , KC_RGHT  , KC_DEL   ,
    KC_ESC   , KC_0     , KC_DOT   , _______  , _______  , KC_BSPC  ,      KC_LNG1  , _______  , _______  , _______  , _______  , _______
  ),

  [2] = LAYOUT_universal(
    S(KC_1)  , S(KC_2)  , S(KC_3)  , S(KC_4)  , S(KC_5)  ,                            S(KC_6)  , S(KC_7)  , S(KC_8)  , S(KC_9)  , S(KC_0)  ,
    KC_1     , KC_2     , KC_3     , KC_4     , KC_5     ,                            KC_6     , KC_7     , KC_8     , KC_9     , KC_0     ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , KC_9     ,                            KC_5     , KC_3     , KC_2     , KC_4     , KC_1     ,
    QK_BOOT  , KC_0     , _______  , KC_N     , KC_LNG2  , _______  ,      _______  , _______  , _______  , _______  , _______  , QK_BOOT
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , EE_CLR   ,                            _______  , _______  , SSNP_HOR , SSNP_VRT , SSNP_FRE ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , SCRL_DVI ,                            _______  , _______  , _______  , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , SCRL_DVD ,                            CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , KBC_SAVE ,
    QK_BOOT  , KBC_RST  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , KBC_RST  , QK_BOOT
  ),

  // クリックレイヤー - トラックボール検知時に自動的に切り替わる
  [4] = LAYOUT_universal(
    _______, _______, _______, _______, _______,                          _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,                          _______, KC_BTN1, KC_BTN3, KC_BTN2, _______,
    _______, _______, _______, _______, _______,                          _______, KC_BTN4, KC_BTN5, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
  ),
};
// clang-format on

// キーボード初期化時の処理
void keyboard_post_init_user(void) {
    // クリックレイヤーの初期化を行う
    auto_click_layer_init();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"


#include "lib/keyball/runcat.h"

void oledkit_render_info_user(void) {
    oled_write_ln_P(PSTR(" "), false);
    oled_write_ln_P(PSTR(" "), false);
    oled_write_ln_P(PSTR(" "), false);
    keyball_oled_render_cat();   // ←ここで猫を描画
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return !is_keyboard_master() ? OLED_ROTATION_180 : OLED_ROTATION_270;
}
//void oledkit_render_info_user(void) {
//   keyball_oled_render_keyinfo();
//   keyball_oled_render_ballinfo();
//   keyball_oled_render_layerinfo();
//}
#endif

// process_record_userとhousekeeping_task_userはクリックレイヤーの実装内で自動的に処理されるため、ここでの定義は不要