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

class Interrupts;

class Controls {
 private:
  map<Button, bool> state;
  map<int, Button> getKeyBindings();

 public:
  Interrupts *interrupts;
  map<Button, int> joypadBindings;
  map<int, Button> keyBindings;
  const map<Button, uint8> buttonToMask;
  uint8 *p1;

  Controls();

  void update();
  void press(int key);
  void release(int key);
  void bind(int key, Button button);
};
