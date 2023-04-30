#pragma once

#include <Qt>
#include <map>
#include <vector>

#include "interrupts.h"
#include "memory.h"
#include "types.h"

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

class Memory;

class Controls {
 private:
  std::map<Button, bool> state;

  uint8 &p1;

  std::map<int, Button> invertKeybindingsMap();

 public:
  std::map<Button, int> keyBindings;
  std::map<int, Button> keyBindingsInv;
  std::map<Button, uint8> buttonToMask;

  Controls(Memory &mem);

  void update();

  void press(int key);
  void release(int key);
  void bind(int key, Button button);
};
