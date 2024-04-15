/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du lanceur de missiles.
 * @version 1.0
 * @date 2023-07-30
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "main.hpp"
#include "pinDefinitions.hpp"
#include "functions.hpp"
#include "remoteControl.hpp"
#include "scheduler.hpp"

// Variables globales.
// En degré par milliseconde.
double baseAngularFrequency;
double angleAngularFrequency;
// En degré.
double baseAngle;
double angleAngle;
double lastBaseAngleUpdateSended;
double lastAngleAngleUpdateSended;
// Sous la forme 010.
String lastMissilesStateUpdateSended;
// En milliseconde.
unsigned long updateInterval;
unsigned long lastUpdateSendedTime;

void setup()
{
    // Définition des modes des broches des capteurs.
    pinMode(PIN_BASE_SENSOR_1, INPUT_PULLUP);
    pinMode(PIN_BASE_SENSOR_2, INPUT_PULLUP);
    pinMode(PIN_MISSILE_1_SENSOR, INPUT_PULLUP);
    pinMode(PIN_MISSILE_2_SENSOR, INPUT_PULLUP);
    pinMode(PIN_MISSILE_3_SENSOR, INPUT_PULLUP);
    pinMode(PIN_LAUNCHER_MOTOR_SENSOR, INPUT_PULLUP);
    pinMode(PIN_ANGLE_SENSOR_1, INPUT_PULLUP);
    pinMode(PIN_ANGLE_SENSOR_2, INPUT_PULLUP);

    // Définition des modes des broches des actionneurs.
    pinMode(PIN_BASE_MOTOR_1, OUTPUT);
    pinMode(PIN_BASE_MOTOR_2, OUTPUT);
    pinMode(PIN_ANGLE_MOTOR_1, OUTPUT);
    pinMode(PIN_ANGLE_MOTOR_2, OUTPUT);
    pinMode(PIN_LAUNCHER_MOTOR_1, OUTPUT);
    pinMode(PIN_LAUNCHER_MOTOR_2, OUTPUT);

    // Lancement des communications UART.
    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE);

    // Initialisation du lance missile : récupération de la position / recalibration si nécessaire.
    start();
}

void loop()
{
    schedule();

    if (Serial.available())
        receivedData(&Serial);

    if (Serial1.available())
        receivedData(&Serial1);
}