#pragma once

#include <QSettings>
#include <map>

#include "mainwindow.h"

// settings keys
#define ROM_PATH_KEY "ROM Path"
#define SCALE_KEY "Scale"
#define PALETTE_KEY "DMG Palette"
#define DEVICE_KEY "Device"
#define SKIP_BOOT_KEY "Skip Bootstrap"

using namespace std;

class Settings {
 private:
  static QSettings settings;

  static QString buttonStr(Button button);

 public:
  // save settings functions
  static void saveRomPath(QString path);
  static void saveScale(float scale);
  static void savePalette(Palette *palette);
  static void saveDevice(bool cgb);
  static void saveSkipDmgBootstrap(bool skip);
  static void saveKeyBinding(int key, Button button);

  // load settings functions
  static void load(MainWindow *mw, map<QString, QAction *> palNameToAction);
};
