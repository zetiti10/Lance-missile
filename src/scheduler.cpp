/**
 * @file scheduler.cpp
 * @author Louis L
 * @brief Fichier de la fonction gérant les moteurs.
 * @version 1.0
 * @date 2023-07-30
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "main.hpp"
#include "pinDefinitions.hpp"
#include "functions.hpp"
#include "scheduler.hpp"
#include "remoteControl.hpp"

void schedule()
{
    if ((upCounter != 0 && upCounter <= millis()) || (downCounter != 0 && downCounter <= millis()))
        stopMotorMove(ANGLE);

    if (upCounter != 0 && digitalRead(PIN_ANGLE_SENSOR_1) == 0)
    {
        stopMotorMove(ANGLE);
        angleAngle = ANGLE_ANGLE;
    }

    if (downCounter != 0 && digitalRead(PIN_ANGLE_SENSOR_2) == 0)
    {
        stopMotorMove(ANGLE);
        angleAngle = 0;
    }

    if ((leftCounter != 0 && leftCounter <= millis()) || (rightCounter != 0 && rightCounter <= millis()))
        stopMotorMove(BASE);

    if (leftCounter != 0 && !checkLeft())
    {
        stopMotorMove(BASE);
        baseAngle = 0;
    }

    if (rightCounter != 0 && !checkRight())
    {
        stopMotorMove(BASE);
        baseAngle = BASE_ANGLE;
    }

    if (motorPositionCounter <= millis())
    {
        if (leftCounter != 0)
            baseAngle -= baseAngularFrequency * MOTOR_POSITION_UPDATE_FREQUENCY;

        if (rightCounter != 0)
            baseAngle += baseAngularFrequency * MOTOR_POSITION_UPDATE_FREQUENCY;

        if (downCounter != 0)
            angleAngle -= angleAngularFrequency * MOTOR_POSITION_UPDATE_FREQUENCY;

        if (upCounter != 0)
            angleAngle += angleAngularFrequency * MOTOR_POSITION_UPDATE_FREQUENCY;

        motorPositionCounter = millis() + MOTOR_POSITION_UPDATE_FREQUENCY;
    }

    if (missileToLaunch > 0)
    {
        int currentAvailableMissiles = checkAvailableMissiles();

        if (currentAvailableMissiles < availableMissiles)
        {
            missileToLaunch--;

            availableMissiles = currentAvailableMissiles;

            if (missileToLaunch == 0)
                launcherCounter = millis() + 5500;
        }

        delay(100);
    }

    if (launcherCounter != 0 && launcherCounter <= millis())
    {
        launcherCounter = 0;

        digitalWrite(PIN_LAUNCHER_MOTOR_1, LOW);
        digitalWrite(PIN_LAUNCHER_MOTOR_2, LOW);
    }

    if (updateInterval != 0 && ((millis() - updateInterval) >= lastUpdateSendedTime))
    {
        if ((baseAngle != lastBaseAngleUpdateSended) || (angleAngle != lastAngleAngleUpdateSended))
        {
            processMessage("50", &Serial);
            processMessage("50", &Serial1);

            lastBaseAngleUpdateSended = baseAngle;
            lastAngleAngleUpdateSended = angleAngle;
        }

        if (getMissilesState() != lastMissilesStateUpdateSended)
        {
            processMessage("51", &Serial);
            processMessage("51", &Serial1);

            lastMissilesStateUpdateSended = getMissilesState();
        }

        lastUpdateSendedTime = millis();
    }
}