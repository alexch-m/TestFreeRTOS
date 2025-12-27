// adjust to your pins
static constexpr uint8_t LedPin1 = 5;
static constexpr uint8_t LedPin2 = 7;

// also test ESP32 LEDC PWM
#define LEDC_PWM 1

void Task1(void*)
{
  while (1)
  {
    digitalWrite(LedPin1, HIGH);
    delay(500);
    digitalWrite(LedPin1, LOW);
    delay(500);
  }
}

#if LEDC_PWM

static constexpr uint8_t LedPwmPin = 4; // adjust to your LED PWM pin
static constexpr int LedPwmFrequency = 5000; // frequency (e.g., 5000 Hz)
static constexpr int LedPwmResolution = 8; // resolution (8-bit gives a range of 0-255)

void Task2(void*)
{
  delay(500);

  while (1)
  {
    // increase brightness (fade in)
    for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
    {
      ledcWrite(LedPwmPin, dutyCycle);
      delay(5);
    }

    delay(1000);

    // decrease brightness (fade out)
    for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
    {
      ledcWrite(LedPwmPin, dutyCycle);
      delay(5);
    }

    delay(500);
  }
}

#endif // LEDC_PWM

void setup()
{
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);
#if LEDC_PWM
  pinMode(LedPwmPin, OUTPUT);
#endif

  Serial.begin(115200);

  if (xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1/*Priority*/, NULL) == pdTRUE)
  {
    Serial.println(F("Task1 started"));
  }
  else
  {
    log_e("xTaskCreate failed");
    for (;;) ;
  }

#if LEDC_PWM

  ledcAttach(LedPwmPin, LedPwmFrequency, LedPwmResolution);

  if (xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1/*Priority*/, NULL) == pdTRUE)
  {
    Serial.println(F("Task2 started"));
  }
  else
  {
    log_e("xTaskCreate failed");
    for (;;) ;
  }

#endif
}

void loop()
{
  digitalWrite(LedPin2, !digitalRead(LedPin2));
  delay(1000);
}
