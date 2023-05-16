// **************************************************
// **************************************************
// **************************************************
// GAME BOY CONTROLS (JOYPAD)
// **************************************************
// **************************************************
// **************************************************

#include "controls.h"

#include "interrupts.h"

// joypad bindings: Button -> int (key)
map<Button, int> Controls::joypadBindings = {
    {RIGHT, Qt::Key_D},  {LEFT, Qt::Key_A}, {UP, Qt::Key_W},
    {DOWN, Qt::Key_S},   {A, Qt::Key_P},    {B, Qt::Key_O},
    {SELECT, Qt::Key_N}, {START, Qt::Key_M}};

// key bindings: int (key) -> Button
// inverse of joypad bindings
map<int, Button> Controls::keyBindings = getKeyBindings();

// map joypad button to mask used to set
// a specific bit in register P1
const map<Button, uint8> Controls::buttonToMask = {
    {RIGHT, RIGHT_MASK},   {LEFT, LEFT_MASK},  {UP, UP_MASK},
    {DOWN, DOWN_MASK},     {A, A_MASK},        {B, B_MASK},
    {SELECT, SELECT_MASK}, {START, START_MASK}};

// current state of controls, separate
// from P1 register
map<Button, bool> Controls::state = {
    {RIGHT, false}, {LEFT, false}, {UP, false},     {DOWN, false},
    {A, false},     {B, false},    {SELECT, false}, {START, false}};

// P1 register pointer
uint8 *Controls::p1 = nullptr;

// update joypad register P1 based on
// the keys currently being pressed on
// the keyboard
//
// for each bit in register P1:
// 0: selected / button pressed
// 1: not selected / button not pressed
//
// P1 register bits
// 7-6: always 1
// 5: select action buttons
// 4: select direction buttons
// 3: down / start
// 2: up / select
// 1: left / b
// 0: right / a
void Controls::update() {
  // save previous value of P1
  uint8 oldP1 = *Controls::p1;

  // set all bits to 1 except for
  // bits 4 and 5
  *Controls::p1 |= ~(BIT4_MASK | BIT5_MASK);

  // select either action buttons or diretion buttons
  vector<Button> buttons = {};
  if (!(*Controls::p1 & BIT4_MASK)) {
    buttons = {RIGHT, LEFT, UP, DOWN};
  } else if (!(*Controls::p1 & BIT5_MASK)) {
    buttons = {A, B, SELECT, START};
  }

  // iterate over each button
  for (auto button : buttons) {
    uint8 mask = Controls::buttonToMask.at(button);

    // if key corresponding to the current
    // button is pressed, set its corresponding
    // bit in P1 to 0
    if (Controls::state[button]) {
      *Controls::p1 &= ~mask;

      // request joypad interrupt if button
      // goes from high to low
      if ((*Controls::p1 & mask) != (oldP1 & mask)) {
        Interrupts::request(JOYPAD_INT);
      }
    }

    // if key corresponding to the current
    // button is not pressed, set its
    // corresponding bit in P1 to 1
    else {
      *Controls::p1 |= mask;
    }
  }
}

// press joypad button
void Controls::press(int key) {
  if (Controls::keyBindings.find(key) != Controls::keyBindings.end()) {
    Controls::state[Controls::keyBindings[key]] = true;
  }
}

// release joypad button
void Controls::release(int key) {
  if (Controls::keyBindings.find(key) != Controls::keyBindings.end()) {
    Controls::state[Controls::keyBindings[key]] = false;
  }
}

// bind specified key to the given joypad button
void Controls::bind(int key, Button button) {
  Controls::joypadBindings[button] = key;
  Controls::keyBindings = Controls::getKeyBindings();
}

// get key bindings map by inverting the
// joypad bindings map
//
// joypadBindings: Button -> int (key)
// keyBindings: int (key) -> Button
map<int, Button> Controls::getKeyBindings() {
  map<int, Button> kb{};
  for (const auto &binding : Controls::joypadBindings) {
    kb[binding.second] = binding.first;
  }
  return kb;
}
