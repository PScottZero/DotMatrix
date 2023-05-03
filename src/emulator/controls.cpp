#include "controls.h"

#include "interrupts.h"

map<Button, int> Controls::keyBindings = {
    {RIGHT, Qt::Key_D},  {LEFT, Qt::Key_A}, {UP, Qt::Key_W},
    {DOWN, Qt::Key_S},   {A, Qt::Key_P},    {B, Qt::Key_O},
    {SELECT, Qt::Key_N}, {START, Qt::Key_M}};

map<int, Button> Controls::keyBindingsInv = invertKeybindingsMap();

const map<Button, uint8> Controls::buttonToMask = {
    {RIGHT, RIGHT_MASK},   {LEFT, LEFT_MASK},  {UP, UP_MASK},
    {DOWN, DOWN_MASK},     {A, A_MASK},        {B, B_MASK},
    {SELECT, SELECT_MASK}, {START, START_MASK}};

map<Button, bool> Controls::state = {
    {RIGHT, false}, {LEFT, false}, {UP, false},     {DOWN, false},
    {A, false},     {B, false},    {SELECT, false}, {START, false}};

uint8 *Controls::p1 = nullptr;

void Controls::update() {
  uint8 oldP1 = *Controls::p1;
  *Controls::p1 |= 0xCF;

  vector<Button> buttons = {};
  if (!(*Controls::p1 & 0x10) || (*Controls::p1 & 0x30) == 0x30) {
    buttons = {RIGHT, LEFT, UP, DOWN};
  } else if (!(*Controls::p1 & 0x20)) {
    buttons = {A, B, SELECT, START};
  }

  for (auto button : buttons) {
    uint8 mask = Controls::buttonToMask.at(button);
    if (Controls::state[button]) {
      *Controls::p1 &= ~mask;

      // request joypad interrupt if button
      // goes from high to low
      if ((*Controls::p1 & mask) != (oldP1 & mask)) {
        Interrupts::request(JOYPAD_INT);
      }
    } else {
      *Controls::p1 |= mask;
    }
  }
}

void Controls::press(int key) {
  if (Controls::keyBindingsInv.find(key) != Controls::keyBindingsInv.end()) {
    Controls::state[Controls::keyBindingsInv[key]] = true;
  }
}

void Controls::release(int key) {
  if (Controls::keyBindingsInv.find(key) != Controls::keyBindingsInv.end()) {
    Controls::state[Controls::keyBindingsInv[key]] = false;
  }
}

void Controls::bind(int key, Button button) {
  Controls::keyBindings[button] = key;
  Controls::keyBindingsInv = Controls::invertKeybindingsMap();
}

map<int, Button> Controls::invertKeybindingsMap() {
  map<int, Button> inv{};
  for (const auto &binding : Controls::keyBindings) {
    inv[binding.second] = binding.first;
  }
  return inv;
}
