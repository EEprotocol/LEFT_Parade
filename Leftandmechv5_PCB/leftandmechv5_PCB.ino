#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>
#include <KeyboardDevice.h>
#include <MouseDevice.h>


KeyboardDevice* keyboard;
MouseDevice* mouse;
BleCompositeHID compositeHID("LEFT Parade", "EEprotocol", 100);

struct KeyCombo {
  const uint8_t* keys;
  uint8_t length;
};
struct keyMapEntry {
  int id;
  KeyCombo combo;
};
const uint8_t KC_SAVE[] = { 0x01, KEY_S };
const uint8_t KC_COPY[] = { 0x01, KEY_C };
const uint8_t KC_PASTE[] = { 0x01, KEY_V };
const uint8_t KC_DELETE[] = { 0x4c };
const uint8_t KC_m[] = { KEY_M };
const uint8_t KC_UNDO[] = { 0x01, KEY_Z };
const uint8_t KC_REDO[] = { 0x01, KEY_Y };
const uint8_t KC_CUT[] = { 0x01, KEY_X };
const uint8_t KC_ESC[] = { KEY_ESC };
const uint8_t KC_UP[] = { KEY_UP };
const uint8_t KC_DOWN[] = { KEY_DOWN };
const uint8_t KC_LEFT[] = { KEY_LEFT };
const uint8_t KC_RIGHT[] = { KEY_RIGHT };
const uint8_t KC_PAGE_NEXT[] = { 0x01, KEY_RIGHT };
const uint8_t KC_PAGE_BACK[] = { 0x01, KEY_LEFT };
const uint8_t KC_SPACE[] = { KEY_SPACE };
const uint8_t KC_PARTIALSHOT[]={KEY_MOD_LMETA,KEY_MOD_LSHIFT,KEY_S};

const keyMapEntry keymap[] = {
 {  1, { KC_SAVE,      2 } },
{  2, { KC_DELETE,    1 } },
{  3, { KC_RIGHT,     1 } },
{  6, { KC_m,         1 } },
{  8, { KC_PAGE_NEXT, 2 } },
{  9, { KC_DOWN,      1 } },
{ 12, { KC_SPACE,     1 } },
{ 18, { KC_UP,        1 } },
{ 20, { KC_PAGE_BACK, 2 } },
{26,{KC_PARTIALSHOT,3}},
{ 27, { KC_LEFT,      1 } },
{ 28, { KC_PASTE,     2 } },
{ 30, { KC_COPY,      2 } },
{ 33, { KC_REDO,      2 } },
{ 36, { KC_CUT,       1 } },
{ 45, { KC_UNDO,      2 } },
{ 54, { KC_ESC,       1 } }
};
const keyMapEntry direct_key[] = {
  { 1, { KC_LEFT, 1 } },
  { 2, { KC_UP, 1 } },
  { 3, { KC_DOWN, 1 } },
  { 5, { KC_RIGHT, 1 } }
};
const int KEYMAP_SIZE = sizeof(keymap) / sizeof(keymap[0]);
const int DIRECT_KEY_SIZE = sizeof(direct_key) / sizeof(direct_key[0]);
const int C1 = D7;
const int C2 = D8;
const int mult1 = D9;
const int mult2 = D10;
const int rota = D4;
const int rotb = D3;
const int rotsw = D2;
const int mult2_1 = D5;
const int mult2_2 = D1;


int stop = 0;
int dir = 0;
unsigned long lastActivityTime = 0;               // 最後にボタンが押された時刻を記録
const unsigned long inactivityThreshold = 60000;  // 1分（60000ミリ秒）操作がない場合にスリープするよう設定
int forward = 0;
int back = 0;
int rot_st = 0;
int total = 0;
int prev_forward = 0;
int prev_back = 0;
bool prev_result[8] = { 0 };
RTC_DATA_ATTR int mode = 1;


bool prev_direct_pressed = false;
bool direct_pressed = false;
void sendKeyCombo(const KeyCombo& combo) {

  // --- modifier press ---
  for (int i = 0; i < combo.length; i++) {
    uint8_t k = combo.keys[i];
    if (k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08) {
      keyboard->modifierKeyPress(k);
    }
  }

  // --- normal key press ---
  for (int i = 0; i < combo.length; i++) {
    uint8_t k = combo.keys[i];
    if (!(k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08)) {
      keyboard->keyPress(k);
    }
  }

  keyboard->sendKeyReport();

  // --- normal key release ---
  for (int i = 0; i < combo.length; i++) {
    uint8_t k = combo.keys[i];
    if (!(k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08)) {
      keyboard->keyRelease(k);
    }
  }

  // --- modifier release ---
  for (int i = 0; i < combo.length; i++) {
    uint8_t k = combo.keys[i];
    if (k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08) {
      keyboard->modifierKeyRelease(k);
    }
  }

  keyboard->sendKeyReport();
}

void sendDirectKeyCombo(const KeyCombo& combo, char order) {
  if (order == 'p') {
    Serial.println("mode1:pushed");
    // --- modifier press ---
    for (int i = 0; i < combo.length; i++) {
      uint8_t k = combo.keys[i];
      if (k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08) {
        keyboard->modifierKeyPress(k);
      }
    }

    // --- normal key press ---
    for (int i = 0; i < combo.length; i++) {
      uint8_t k = combo.keys[i];
      if (!(k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08)) {
        keyboard->keyPress(k);
      }
    }

    keyboard->sendKeyReport();
  }

  if (order == 'r') {
    for (int k = 0; k < 100; k++) {


      Serial.println("mode2:released");
    }
    // --- normal key release ---
    for (int i = 0; i < combo.length; i++) {
      uint8_t k = combo.keys[i];
      if (!(k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08)) {
        keyboard->keyRelease(k);
      }
    }

    // --- modifier release ---
    for (int i = 0; i < combo.length; i++) {
      uint8_t k = combo.keys[i];
      if (k == 0x01 || k == 0x02 || k == 0x04 || k == 0x08) {
        keyboard->modifierKeyRelease(k);
      }
    }

    keyboard->sendKeyReport();
  }
}

bool execKeymap(int id) {
  for (int i = 0; i < KEYMAP_SIZE; i++) {
    if (keymap[i].id == id) {
      sendKeyCombo(keymap[i].combo);
      return true;  // 実行成功
    }
  }
  return false;  // 該当IDなし
}

bool execDirectAttack(int id, char order) {
  for (int i = 0; i < DIRECT_KEY_SIZE; i++) {
    if (direct_key[i].id == id) {
      sendDirectKeyCombo(direct_key[i].combo, order);
      return true;  // 実行成功
    }
  }
  return false;  // 該当IDなし
}

void checkPos() {
  int reada = digitalRead(rota);
  int readb = digitalRead(rotb);

  if (readb == reada) {
    //Serial.println("STOP");
  } else {
    if (reada > readb) {
      dir = stop;
    } else {
      dir = -1 * stop;
    }
  }
}
void pushrotsw() {
  if (digitalRead(rotsw) == 0) {
    rot_st = 1;
    Serial.println("rotsw_pushed");
    if (mode == 1) {
      mode = 2;
    } else if (mode == 2) {
      mode = 1;
    }
  }
}




void allPinsHiZ() {
  pinMode(mult1, OUTPUT);
  pinMode(mult2, OUTPUT);
  pinMode(C1, INPUT_PULLUP);
  pinMode(C2, INPUT_PULLUP);

  digitalWrite(mult1, LOW);
  digitalWrite(mult2, LOW);
  delayMicroseconds(10);
}

void scanMatrix(int k[8]) {
  allPinsHiZ();
  for (int i = 0; i < 4; i++) {
    digitalWrite(mult1, i & 1);
    digitalWrite(mult2, (i >> 1) & 1);
    delayMicroseconds(3);
    k[2 * i] = (digitalRead(C2) == LOW);
    k[2 * i + 1] = (digitalRead(C1) == LOW);
  }
}                                       
void setup() {
  Serial.begin(115200);
  pinMode(rota, INPUT_PULLUP);
  pinMode(rotb, INPUT_PULLUP);
  pinMode(rotsw, INPUT_PULLUP);
  pinMode(mult2_1, OUTPUT);
  pinMode(mult2_2, OUTPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(rota), checkPos, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotb), checkPos, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotsw), pushrotsw, CHANGE);
  // Set up keyboard
  KeyboardConfiguration keyboardConfig;
  keyboardConfig.setAutoReport(false);
  keyboard = new KeyboardDevice(keyboardConfig);

  // Set up mouse
  MouseConfiguration mouseConfig;
  mouseConfig.setAutoReport(false);
  mouse = new MouseDevice(mouseConfig);

  // Add both devices to the composite HID device to manage them
  compositeHID.addDevice(keyboard);
  compositeHID.addDevice(mouse);

  // Start the composite HID device to broadcast HID reports
  compositeHID.begin();
  esp_deep_sleep_enable_gpio_wakeup(
    BIT(4), ESP_GPIO_WAKEUP_GPIO_LOW);

  delay(3000);
}


bool operated = 0;
bool sleeping = false;
unsigned long last_operated = 0;
bool recorder[8] = { 0 };
int sum = 0;
int prevsum = 0;
void loop() {
  digitalWrite(mult2_2, mode & 1);
  digitalWrite(mult2_1, (mode >> 1) & 1);
  int keys[8] = { 0 };
  scanMatrix(keys);
  bool flag_pushed = 0;
  int bitnum = 0;
  Serial.print("mode:");
  Serial.println(mode);
  int tmp[8]={0};
  for(int i=0;i<8;i++){
    tmp[7-i]=keys[i];
  }
  for(int i=0;i<4;i++){
    int j=tmp[2*i];
    tmp[2*i]=tmp[2*i+1];
    tmp[2*i+1]=j;
  }
  for(int i=0;i<8;i++){
    keys[i]=tmp[i];
  }


  for (int i = 0; i < 8; i++) {
    Serial.print(keys[i] ? "1 " : "0 ");
    bitnum += keys[i];
    flag_pushed = (flag_pushed || keys[i]);
  }
  Serial.println();


  if (flag_pushed == 1) {
    for (int i = 0; i < 8; i++) {
      recorder[i] = (recorder[i] || keys[i]);
      Serial.print(recorder[i]);
    }
    Serial.println();
  } else {
    sum = 0;
    for (int i = 0; i < 4; i++) {
      int tmp = 2 * recorder[2 * i] + recorder[2 * i + 1];
      sum += pow(3, 3 - i) * tmp;
      if (recorder[2 * i] == 1 && recorder[2 * i + 1] == 1) {
        sum = -1;
        break;
      }
    }
    Serial.print("sum= ");
    Serial.println(sum);
    for (int i = 0; i < 8; i++) {
      recorder[i] = 0;
    }
    if (sum != 0) {
      prevsum = sum;
    }
    Serial.print("prevsum= ");
    Serial.println(prevsum);
  }

  int reada = digitalRead(rota);
  int readb = digitalRead(rotb);
  int newstop = 0;

  if (reada == readb) {
    if (readb == 0) {
      newstop = -1;
    } else {
      newstop = 1;
    }
    if (dir != 0 && newstop != stop) {
      if (dir == 1) {
        //Serial.println("UP");
        total++;
        forward = 1;
      } else if (dir == -1) {
        total--;
        back = 1;
        //Serial.println("DOWN");
      }
      Serial.println(total);
      dir = 0;
    }
    stop = newstop;
  }

  for (int i = 0; i < 8; i++) {
    if (keys[i] == 1) {
      operated = true;
    }
  }
  if (compositeHID.isConnected()) {
    if(mode==0){
      mode=1;
    }
    if (mode == 1 && sum != 0) {
      execKeymap(sum);
      sum = 0;
    } else if (mode == 2) {
      direct_pressed = false;
      for (int i = 0; i < 8; i++) {
        if (keys[i] == 1 && prev_result[i] == 0) {

          execDirectAttack(i, 'p');
          prev_result[i]=1;
        }
      }
      for (int i = 0; i < 8; i++) {
        if (keys[i] == 0 && prev_result[i] == 1) {
          for (int k = 0; k < 10000; k++) {

            Serial.println("mode2:released");

          }
          execDirectAttack(i, 'r');
          prev_result[i]=0;
        }
      }
    }
    if (forward == 1) {
      operated = true;

      if (millis() - prev_forward < 200) {
        mouse->mouseMove(0, 0, 0, 3);
        mouse->sendMouseReport();
      }
      mouse->mouseMove(0, 0, 0, 3);
      mouse->sendMouseReport();

      prev_forward = millis();
    }
    if (back == 1) {
      operated = true;

      if (millis() - prev_back < 200) {
        mouse->mouseMove(0, 0, 0, -3);
        mouse->sendMouseReport();
      }
      mouse->mouseMove(0, 0, 0, -3);
      mouse->sendMouseReport();

      prev_back = millis();
    }
    if (rot_st) {
      operated = true;
      Serial.println("SW pushed!");
      lastActivityTime = millis();

      rot_st = 0;
    }
  }else{
    mode=0;
  }
  Serial.println(operated);
  Serial.print("last_operated: ");
  Serial.println(last_operated);
  if (forward || back || operated) {
    Serial.print("forward= ");
    Serial.print(forward);
    Serial.print(" back= ");
    Serial.print(back);
    Serial.print(" operated= ");
    Serial.println(operated);
    last_operated = millis();
    operated = false;
    forward = 0;
    back = 0;

  } else {
    if (millis() - last_operated > 300000) {
      digitalWrite(mult2_1, 0);
      digitalWrite(mult2_2, 0);
      delay(1000);
      Serial.println("into deep sleep");
      Serial.flush();
      esp_deep_sleep_start();
    }
  }


  delay(20);
}