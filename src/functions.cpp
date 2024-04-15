/**
 * @file functions.cpp
 * @author Louis L
 * @brief Fichier de fonctions utiles au programme.
 * @version 1.0
 * @date 2023-07-30
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>
#include <remoteControl.hpp>

// Autres fichiers du programme.
#include "functions.hpp"
#include "main.hpp"
#include "pinDefinitions.hpp"

// Variables globales.
unsigned long upCounter = 0;
unsigned long downCounter = 0;
unsigned long leftCounter = 0;
unsigned long rightCounter = 0;
int availableMissiles = 0;
int missileToLaunch = 0;
unsigned long launcherCounter = 0;
unsigned long motorPositionCounter = 0;

void motorMoveTimer(int move, unsigned long time)
{
    if (move == UP)
    {
        if (digitalRead(PIN_ANGLE_SENSOR_1) == 0)
            return;

        digitalWrite(PIN_ANGLE_MOTOR_1, HIGH);
        digitalWrite(PIN_ANGLE_MOTOR_2, LOW);
        upCounter = millis() + time;
        downCounter = 0;
    }

    else if (move == DOWN)
    {
        if (digitalRead(PIN_ANGLE_SENSOR_2) == 0)
            return;

        digitalWrite(PIN_ANGLE_MOTOR_1, LOW);
        digitalWrite(PIN_ANGLE_MOTOR_2, HIGH);
        downCounter = millis() + time;
        upCounter = 0;
    }

    else if (move == LEFT)
    {
        if (digitalRead(PIN_BASE_SENSOR_2) == 0)
            return;

        digitalWrite(PIN_BASE_MOTOR_1, LOW);
        digitalWrite(PIN_BASE_MOTOR_2, HIGH);
        leftCounter = millis() + time;
        rightCounter = 0;
    }

    else if (move == RIGHT)
    {
        if (digitalRead(PIN_BASE_SENSOR_1) == 0)
            return;

        digitalWrite(PIN_BASE_MOTOR_1, HIGH);
        digitalWrite(PIN_BASE_MOTOR_2, LOW);
        rightCounter = millis() + time;
        leftCounter = 0;
    }
}

void motorMoveRelative(int direction, int angle)
{
    if (direction == BASE)
    {
        if (angle > BASE_ANGLE)
            angle = BASE_ANGLE;

        else if (angle < -BASE_ANGLE)
            angle = -BASE_ANGLE;

        if (angle == BASE_ANGLE)
            startMotorMove(RIGHT);

        else if (angle == -BASE_ANGLE)
            startMotorMove(LEFT);

        else if (angle > 0)
            motorMoveTimer(RIGHT, angle / baseAngularFrequency);

        else if (angle < 0)
            motorMoveTimer(LEFT, abs(angle) / baseAngularFrequency);
    }

    else if (direction == ANGLE)
    {
        if (angle > ANGLE_ANGLE)
            angle = ANGLE_ANGLE;

        else if (angle < -ANGLE_ANGLE)
            angle = -ANGLE_ANGLE;

        if (angle == ANGLE_ANGLE)
            startMotorMove(UP);

        else if (angle == -ANGLE_ANGLE)
            startMotorMove(DOWN);

        else if (angle > 0)
            motorMoveTimer(UP, angle / angleAngularFrequency);

        else if (angle < 0)
            motorMoveTimer(DOWN, abs(angle) / angleAngularFrequency);
    }
}

void motorMoveAbsolute(int direction, int angle)
{
    if (direction == BASE)
    {
        if (angle > BASE_ANGLE)
            angle = BASE_ANGLE;

        else if (angle < 0)
            angle = 0;

        if (angle == BASE_ANGLE)
            startMotorMove(RIGHT);

        else if (angle == 0)
            startMotorMove(LEFT);

        else if (angle < baseAngle)
            motorMoveTimer(LEFT, (baseAngle - angle) / baseAngularFrequency);

        else if (angle > baseAngle)
            motorMoveTimer(RIGHT, (angle - baseAngle) / baseAngularFrequency);
    }

    else if (direction == ANGLE)
    {
        if (angle > ANGLE_ANGLE)
            angle = ANGLE_ANGLE;

        else if (angle < 0)
            angle = 0;

        if (angle == ANGLE_ANGLE)
            startMotorMove(UP);

        else if (angle == 0)
            startMotorMove(DOWN);

        else if (angle < angleAngle)
            motorMoveTimer(DOWN, (angleAngle - angle) / angleAngularFrequency);

        else if (angle > angleAngle)
            motorMoveTimer(UP, (angle - angleAngle) / angleAngularFrequency);
    }
}

void startMotorMove(int move)
{
    motorMoveTimer(move, 100000);
}

void stopMotorMove(int axis)
{
    if (axis == BASE)
    {
        leftCounter = 0;
        rightCounter = 0;

        digitalWrite(PIN_BASE_MOTOR_1, LOW);
        digitalWrite(PIN_BASE_MOTOR_2, LOW);
    }

    else if (axis == ANGLE)
    {
        upCounter = 0;
        downCounter = 0;

        digitalWrite(PIN_ANGLE_MOTOR_1, LOW);
        digitalWrite(PIN_ANGLE_MOTOR_2, LOW);
    }
}

boolean checkLeft()
{
    if (digitalRead(PIN_BASE_SENSOR_2) == 0)
    {
        delay(1);
        if (digitalRead(PIN_BASE_SENSOR_2) == 0)
            return false;
    }

    return true;
}

boolean checkRight()
{
    if (digitalRead(PIN_BASE_SENSOR_1) == 0)
    {
        delay(1);
        if (digitalRead(PIN_BASE_SENSOR_1) == 0)
            return false;
    }

    return true;
}

void calibrate()
{
    // Calibration de l'orientation de la base.
    unsigned long initialTime = millis();

    // Retour à l'origine.
    startMotorMove(RIGHT);

    while (checkRight())
        checkMessages();

    // Démarrage de la mesure.
    unsigned long middleTime = millis();

    startMotorMove(LEFT);

    while (checkLeft())
        checkMessages();

    unsigned long finalTime = millis();
    stopMotorMove(BASE);

    // Calcul de la vitesse angulaire.
    unsigned long time = finalTime - middleTime;
    baseAngularFrequency = double(BASE_ANGLE) / double(time);

    // Retour à la position initiale.
    unsigned long moveTime = time - (middleTime - initialTime);

    startMotorMove(RIGHT);

    while (millis() - finalTime <= moveTime && checkRight())
        checkMessages();

    baseAngle = baseAngularFrequency * (millis() - finalTime);
    stopMotorMove(BASE);

    // Calibration de l'inclinaison de la tête.
    initialTime = millis();

    // Retour à l'origine.
    startMotorMove(UP);

    while (digitalRead(PIN_ANGLE_SENSOR_1) == 1)
        checkMessages();

    // Démarrage de la mesure.
    middleTime = millis();

    startMotorMove(DOWN);

    while (digitalRead(PIN_ANGLE_SENSOR_2) == 1)
        checkMessages();

    finalTime = millis();
    stopMotorMove(ANGLE);

    // Calcul de la vitesse angulaire.
    time = finalTime - middleTime;
    angleAngularFrequency = double(ANGLE_ANGLE) / double(time);

    // Retour à la position initiale.
    moveTime = time - (middleTime - initialTime);

    startMotorMove(UP);

    while (millis() - finalTime <= moveTime && digitalRead(PIN_ANGLE_SENSOR_1) == 1)
        checkMessages();

    angleAngle = angleAngularFrequency * (millis() - finalTime);
    stopMotorMove(ANGLE);

    // Enregistrement des vitesses angulaires dans la mémoire persistante.
    EEPROM.put(ANGLE_ANGULAR_FREQUENCY_STORAGE_LOCATION, angleAngularFrequency);
    EEPROM.put(BASE_ANGULAR_FREQUENCY_STORAGE_LOCATION, baseAngularFrequency);
}

void start()
{
    if (isnan(EEPROM.read(BASE_ANGULAR_FREQUENCY_STORAGE_LOCATION)) || isnan(EEPROM.read(ANGLE_ANGULAR_FREQUENCY_STORAGE_LOCATION)))
        calibrate();

    else
    {
        EEPROM.get(ANGLE_ANGULAR_FREQUENCY_STORAGE_LOCATION, angleAngularFrequency);
        EEPROM.get(BASE_ANGULAR_FREQUENCY_STORAGE_LOCATION, baseAngularFrequency);

        // Mesure de l'angle de la base.
        unsigned long initialTime = millis();

        startMotorMove(LEFT);

        while (checkLeft())
            checkMessages();

        unsigned long finalTime = millis();

        stopMotorMove(BASE);

        unsigned long moveTime = finalTime - initialTime;

        // Retour à l'angle initial.
        startMotorMove(RIGHT);

        while (millis() - finalTime <= moveTime && checkRight())
            checkMessages();

        stopMotorMove(BASE);

        baseAngle = baseAngularFrequency * (millis() - finalTime);

        // Mesure de l'angle de l'inclinaison.
        initialTime = millis();

        startMotorMove(DOWN);

        while (digitalRead(PIN_ANGLE_SENSOR_2) == 1)
            checkMessages();

        finalTime = millis();

        stopMotorMove(ANGLE);

        moveTime = finalTime - initialTime;

        // Retour à l'angle initial.
        startMotorMove(UP);

        while (millis() - finalTime <= moveTime && digitalRead(PIN_ANGLE_SENSOR_1) == 1)
            checkMessages();

        stopMotorMove(ANGLE);

        angleAngle = baseAngularFrequency * (millis() - finalTime);
    }

    lastBaseAngleUpdateSended = baseAngle;
    lastAngleAngleUpdateSended = angleAngle;
    lastMissilesStateUpdateSended = getMissilesState();
    lastUpdateSendedTime = 0;
    updateInterval = 0;
}

int checkAvailableMissiles()
{
    int currentAvailableMissiles = 0;

    if (digitalRead(PIN_MISSILE_1_SENSOR) == 0)
        currentAvailableMissiles++;

    if (digitalRead(PIN_MISSILE_2_SENSOR) == 0)
        currentAvailableMissiles++;

    if (digitalRead(PIN_MISSILE_3_SENSOR) == 0)
        currentAvailableMissiles++;

    return currentAvailableMissiles;
}

String getMissilesState()
{
    String state;

    if (digitalRead(PIN_MISSILE_1_SENSOR) == 0)
        state += '1';

    else
        state += '0';

    if (digitalRead(PIN_MISSILE_2_SENSOR) == 0)
        state += '1';

    else
        state += '0';

    if (digitalRead(PIN_MISSILE_3_SENSOR) == 0)
        state += '1';

    else
        state += '0';

    return state;
}

void missileLaunch(int number)
{
    if (number == 0)
        return;

    availableMissiles = checkAvailableMissiles();

    if (availableMissiles == 0)
        return;

    number += missileToLaunch;

    if (number > availableMissiles)
        number = availableMissiles;

    missileToLaunch = number;

    launcherCounter = 0;

    digitalWrite(PIN_LAUNCHER_MOTOR_1, LOW);
    digitalWrite(PIN_LAUNCHER_MOTOR_2, HIGH);
}