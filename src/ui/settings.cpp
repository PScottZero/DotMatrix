#include "settings.h"

QSettings Settings::settings("PScottZero", "Dot Matrix");

void Settings::saveRomPath(QString path) {
  settings.setValue(ROM_PATH_KEY, path);
}

void Settings::saveScale(float scale) { settings.setValue(SCALE_KEY, scale); }

void Settings::savePalette(Palette *palette) {
  settings.setValue(PALETTE_KEY, palette->name);
}

void Settings::saveDevice(bool cgb) {
  settings.setValue(DEVICE_KEY, cgb ? "CGB" : "DMG");
}

void Settings::saveSkipDmgBootstrap(bool skip) {
  settings.setValue(SKIP_BOOT_KEY, skip);
}

void Settings::saveKeyBinding(int key, Button button) {
  settings.setValue(buttonStr(button), key);
}

void Settings::load(MainWindow *mw, map<QString, QAction *> palNameToAction) {
  // set rom path setting
  if (settings.contains(ROM_PATH_KEY)) {
    mw->cgb.romPath = settings.value(ROM_PATH_KEY).toString();
  }

  // set scale setting
  float scale = 1.0;
  if (settings.contains(SCALE_KEY)) {
    scale = settings.value(SCALE_KEY).toFloat();
    if (scale == 0.5) {
      mw->ui->action0_5x->setChecked(true);
    } else if (scale == 1.0) {
      mw->ui->action1x->setChecked(true);
    } else if (scale == 1.5) {
      mw->ui->action1_5x->setChecked(true);
    } else if (scale == 2.0) {
      mw->ui->action2x->setChecked(true);
    }
  }
  mw->setScale(scale);

  // set palette setting
  if (settings.contains(PALETTE_KEY)) {
    auto palName = settings.value(PALETTE_KEY).toString();
    palNameToAction[palName]->setChecked(true);
  }

  // set device setting
  if (settings.contains(DEVICE_KEY)) {
    auto device = settings.value(DEVICE_KEY).toString();
    mw->cgb.cgbMode = device == "CGB";
    if (device == "CGB") {
      mw->ui->actionGameBoyColor->setChecked(true);
    } else {
      mw->ui->actionGameBoy->setChecked(true);
    }
  }

  // set skip dmg bootstrap setting
  if (settings.contains(SKIP_BOOT_KEY)) {
    bool skip = settings.value(SKIP_BOOT_KEY).toBool();
    mw->cgb.bootstrap.skipDmg = skip;
    mw->ui->actionSkipDmgBootstrap->setChecked(skip);
  }

  // key binding settings
  Button buttons[8] = {RIGHT, LEFT, UP, DOWN, A, B, SELECT, START};
  for (auto button : buttons) {
    auto buttonName = buttonStr(button);
    if (settings.contains(buttonName)) {
      int key = settings.value(buttonName).toInt();
      mw->cgb.controls.bind(key, button);
    }
  }
}

QString Settings::buttonStr(Button button) {
  switch (button) {
    case RIGHT:
      return "Right";
    case LEFT:
      return "Left";
    case UP:
      return "Up";
    case DOWN:
      return "Down";
    case A:
      return "A";
    case B:
      return "B";
    case SELECT:
      return "Select";
    case START:
      return "Start";
    default:
      return "";
  }
}
