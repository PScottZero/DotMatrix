#include "controls.h"

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
  p1 |= 0xCF;

  std::vector<Button> buttons = {};
  if (((p1 >> 4) & 0b1) == 0) {
    buttons = {RIGHT, LEFT, UP, DOWN};
  } else if (((p1 >> 5) & 0b1) == 0) {
    buttons = {A, B, SELECT, START};
  }

  for (auto button : buttons) {
    if (state[button]) {
      p1 &= ~buttonToMask[button];
    } else {
      p1 |= buttonToMask[button];
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
