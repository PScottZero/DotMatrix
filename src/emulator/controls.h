// **************************************************
// **************************************************
// **************************************************
// Controls (Joypad)
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
#define RIGHT_A_MASK 0x1
#define LEFT_B_MASK 0x2
#define UP_SELECT_MASK 0x4
#define DOWN_START_MASK 0x8

class CGB;

class Controls {
 private:
  map<Button, bool> state;
  map<int, Button> getKeyBindings();

 public:
  CGB *cgb;
  map<Button, int> joypadBindings;
  map<int, Button> keyBindings;
  const map<Button, uint8> buttonToMask;

  Controls();

  void update();
  void press(int key);
  void release(int key);
  void bind(int key, Button button);
};
