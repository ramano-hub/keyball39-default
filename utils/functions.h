// 自前の絶対数を返す関数
static inline int16_t my_abs(int16_t num) {
  if (num < 0) {
    num = -num;
  }
  return num;
}