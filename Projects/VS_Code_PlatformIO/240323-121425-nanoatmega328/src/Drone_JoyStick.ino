#define AngleX A0  // Підключення піна для вимірювання кута X (Connection of the pin for measuring angle X)
#define AngleY A1  // Підключення піна для вимірювання кута Y (Connection of the pin for measuring angle Y)

#define DriveZ A2  // Підключення піна для управління осьовою швидкістю Z (Connection of the pin for controlling axial speed Z)
#define DriveR A3  // Підключення піна для управління обертовою швидкістю R (Connection of the pin for controlling rotational speed R)

#define Butt_A 2
#define Butt_D 3

#define ButtonS_ADC A4

#define NRF_CS 4
#define NRF_CE 5

uint16_t
  data_send[6] = {0},
  data_check[4] = {0};

byte
  joystick_pins[4] = {DriveZ, DriveR, AngleY, AngleX};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 4; i++) {
    pinMode(joystick_pins[i], INPUT);
  }
  pinMode(Butt_A, INPUT_PULLUP);
  pinMode(Butt_D, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 4; i++) {
    data_check[i] = analogRead(joystick_pins[i]);
    data_send [i] = (data_send[i] != data_check[i])?data_check[i]:data_send[i];
  }
}
