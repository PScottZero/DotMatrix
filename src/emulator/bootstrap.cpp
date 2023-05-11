// **************************************************
// **************************************************
// **************************************************
// BOOTSTRAP (RUNS WHEN GAME BOY BOOTS)
// **************************************************
// **************************************************
// **************************************************

#include "bootstrap.h"

#include <QFile>

#include "cgb.h"

// bootstrap status flags
bool Bootstrap::enabled = true;
bool Bootstrap::skip = true;

// dmg bootstrap bytes (256 bytes)
uint8 Bootstrap::dmgBootstrap[DMG_BOOTSTRAP_BYTES]{};

// cgb bootstrap bytes (2304 bytes)
uint8 Bootstrap::cgbBootstrap[CGB_BOOTSTRAP_BYTES]{};

void Bootstrap::loadBootstrap(bool dmg) {
  if (dmg) {
    QFile bootstrap(":/assets/bin/dmg_boot.bin");
    bootstrap.read((char *)dmgBootstrap, DMG_BOOTSTRAP_BYTES);
  } else {
    QFile bootstrap(":/assets/bin/cgb_boot.bin");
    bootstrap.read((char *)cgbBootstrap, CGB_BOOTSTRAP_BYTES);
  }
}

// get byte of bootstrap at given address
uint8 &Bootstrap::at(uint16 addr) {
  if (CGB::dmgMode) {
    return Bootstrap::dmgBootstrap[addr];
  } else {
    return Bootstrap::cgbBootstrap[addr];
  }
}

// check if bootstrap is enabled and should be
// fast-forwarded through
bool Bootstrap::enabledAndShouldSkip() {
  return Bootstrap::enabled && Bootstrap::skip;
}

// reset bootstrap
void Bootstrap::reset() {
  Bootstrap::enabled = true;
  Bootstrap::loadBootstrap(CGB::dmgMode);
}
