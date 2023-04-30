#include "controls.h"

#include "interrupts.h"

Controls::Controls(Memory &mem)
    : keyBindings({{RIGHT, Qt::Key_D},
                   {LEFT, Qt::Key_A},
                   {UP, Qt::Key_W},
                   {DOWN, Qt::Key_S},
                   {A, Qt::Key_P},
                   {B, Qt::Key_O},
                   {SELECT, Qt::Key_N},
                   {START, Qt::Key_M}}),
      keyBindingsInv(invertKeybindingsMap()),
      state({{RIGHT, false},
             {LEFT, false},
             {UP, false},
             {DOWN, false},
             {A, false},
             {B, false},
             {SELECT, false},
             {START, false}}),
      buttonToMask({{RIGHT, RIGHT_MASK},
                    {LEFT, LEFT_MASK},
                    {UP, UP_MASK},
                    {DOWN, DOWN_MASK},
                    {A, A_MASK},
                    {B, B_MASK},
                    {SELECT, SELECT_MASK},
                    {START, START_MASK}}),
      p1(mem.getByte(P1)) {}

void Controls::update() {
  uint8 oldP1 = p1;
  p1 |= 0xCF;

  std::vector<Button> buttons = {};
  if (!(p1 & 0x10) || ((p1 & 0x30) == 0x30)) {
    buttons = {RIGHT, LEFT, UP, DOWN};
  } else if (!(p1 & 0x20)) {
    buttons = {A, B, SELECT, START};
  }

  for (auto button : buttons) {
    uint8 mask = buttonToMask[button];
    if (state[button]) {
      p1 &= ~mask;

      // request joypad interrupt if button
      // goes from high to low
      if ((p1 & mask) != (oldP1 & mask)) {
        Interrupts::request(JOYPAD_INT);
      }
    } else {
      p1 |= mask;
    }
  }
}

void Controls::press(int key) {
  if (keyBindingsInv.find(key) != keyBindingsInv.end()) {
    state[keyBindingsInv[key]] = true;
  }
}

void Controls::release(int key) {
  if (keyBindingsInv.find(key) != keyBindingsInv.end()) {
    state[keyBindingsInv[key]] = false;
  }
}

void Controls::bind(int key, Button button) {
  keyBindings[button] = key;
  keyBindingsInv = invertKeybindingsMap();
}

std::map<int, Button> Controls::invertKeybindingsMap() {
  std::map<int, Button> inv{};
  for (const auto &binding : keyBindings) {
    inv[binding.second] = binding.first;
  }
  return inv;
}
