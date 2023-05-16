// **************************************************
// **************************************************
// **************************************************
// GAME BOY CONTROLS (JOYPAD)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <Qt>
#include <map>
#include <vector>

#include "types.h"

using namespace std;

// joypad buttons
enum Button { RIGHT, LEFT, UP, DOWN, A, B, SELECT, START };

// joypad + button constants
#define RIGHT_MASK 0x1
#define LEFT_MASK 0x2
#define UP_MASK 0x4
#define DOWN_MASK 0x8
#define A_MASK 0x1
#define B_MASK 0x2
#define SELECT_MASK 0x4
#define START_MASK 0x8

class Controls {
 private:
  static map<Button, bool> state;
  static map<int, Button> getKeyBindings();

 public:
  static map<int, Button> keyBindings;
  static map<Button, int> joypadBindings;
  const static map<Button, uint8> buttonToMask;
  static uint8 *p1;

  static void update();
  static void press(int key);
  static void release(int key);
  static void bind(int key, Button button);
};
