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
        delay(5);
        if (digitalRead(PIN_BASE_SENSOR_2) == 0)
            return false;
    }

    return true;
}

boolean checkRight()
{
    if (digitalRead(PIN_BASE_SENSOR_1) == 0)
    {
        delay(5);
        if (digitalRead(PIN_BASE_SENSOR_1) == 0)
            return false;
    }

    return true;
}

void calibrate()
{
    // Calibration de l'orientation de la base.
    // Retour à l'origine.
    startMotorMove(RIGHT);

    while (checkRight())
        delay(1);

    // Démarrage de la mesure.
    unsigned long initialTime = millis();

    startMotorMove(LEFT);

    while (checkLeft())
        delay(1);

    unsigned long finalTime = millis();
    baseAngle = 0;
    stopMotorMove(BASE);

    // Calcul de la vitesse angulaire.
    unsigned long time = finalTime - initialTime;
    baseAngularFrequency = double(BASE_ANGLE) / double(time);

    // Calibration de l'inclinaison de la tête.
    startMotorMove(UP);

    while (digitalRead(PIN_ANGLE_SENSOR_1) == 1)
        delay(1);

    // Démarrage de la mesure.
    initialTime = millis();

    startMotorMove(DOWN);

    while (digitalRead(PIN_ANGLE_SENSOR_2) == 1)
        delay(1);

    finalTime = millis();
    angleAngle = 0;
    stopMotorMove(ANGLE);

    // Calcul de la vitesse angulaire.
    time = finalTime - initialTime;
    angleAngularFrequency = double(ANGLE_ANGLE) / double(time);

    // Enregistrement des vitesses angulaires dans la mémoire persistante.
    EEPROM.write(0, baseAngularFrequency);
    EEPROM.write(1, angleAngularFrequency);
}

void start()
{
    if (EEPROM.read(0) == 255 || EEPROM.read(1) == 255)
        calibrate();

    else
    {
        baseAngularFrequency = EEPROM.read(0);
        angleAngularFrequency = EEPROM.read(1);

        // Mesure de l'angle de la base.
        unsigned long initialTime = millis();

        startMotorMove(LEFT);

        while (checkLeft())
            delay(1);

        unsigned long finalTime = millis();

        stopMotorMove(BASE);

        unsigned long baseTime = finalTime - initialTime;

        // Mesure de l'angle de l'inclinaison.
        initialTime = millis();

        startMotorMove(DOWN);

        while (digitalRead(PIN_ANGLE_SENSOR_2) == 1)
            delay(1);

        finalTime = millis();

        stopMotorMove(ANGLE);

        unsigned long angleTime = finalTime - initialTime;

        motorMoveTimer(RIGHT, baseAngle);
        motorMoveTimer(UP, angleTime);
    }
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