#include <utils.hpp>

// Function to turn off all LEDs
void turnOffLEDs()
{
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
}

void turnOffLedsAndBuzzer()
{
    // Turn off all LEDs
    turnOffLEDs();

    // Turn off the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
}

void playMelody(int note, int duration, int delayP)
{
    tone(BUZZER_PIN, note, duration);
    delay(delayP);
}

// Function to play the goodbye melody
void goodbyeMelody()
{
    // Ensure buzzer and LEDs is off initially
    turnOffLedsAndBuzzer();

    // Turn the LED blue
    analogWrite(BLUE_PIN, 255);

    // Play the melody
    playMelody(NOTE_G5, 200, 250);
    playMelody(NOTE_E5, 200, 250);
    playMelody(NOTE_C5, 200, 250);

    turnOffLedsAndBuzzer();
}

void turnLedYellow()
{
    // Turn off all LEDs
    turnOffLEDs();

    // Turn the LED yellow
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 100);
}

// Function to play the admin goodbye melody
void adminGoodbyeMelody()
{
    // Turn the LED blue
    turnOffLedsAndBuzzer();

    // Turn the LED yellow
    turnLedYellow();

    // Play the melody
    playMelody(NOTE_C5, 200, 250);
    playMelody(NOTE_G5, 200, 250);
    playMelody(NOTE_E5, 200, 250);

    turnOffLedsAndBuzzer();
}

// Function to play the admin access melody
void adminAccessMelody()
{
    // Turn off all LEDs
    turnOffLedsAndBuzzer();

    // Turn the LED yellow
    turnLedYellow();

    // Play the melody
    playMelody(NOTE_G5, 250, 350);
    playMelody(NOTE_E5, 250, 350);

    // Stop leds and buzzer
    turnOffLedsAndBuzzer();
}

// Function to play the access granted melody
void accessGrantedMelody()
{
    // Ensure buzzer is off initially
    turnOffLedsAndBuzzer();
    analogWrite(GREEN_PIN, 255);

    // Play the melody
    playMelody(NOTE_G5, 250, 350);
    playMelody(NOTE_E5, 250, 350);

    turnOffLedsAndBuzzer();
}

// Function to play the access denied melody
void accessDeniedMelody()
{
    // Turn the LED red
    turnOffLedsAndBuzzer();

    // Turn the LED red
    analogWrite(RED_PIN, 255);

    // Play the melody
    playMelody(NOTE_G4, 150, 150);
    playMelody(NOTE_C4, 150, 150);

    turnOffLedsAndBuzzer();
}
