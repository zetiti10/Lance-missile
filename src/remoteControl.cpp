/**
 * @file remoteControl.cpp
 * @author Louis L
 * @brief Fichier de fonctions utiles au programme.
 * @version 1.0
 * @date 2023-07-30
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "remoteControl.hpp"
#include "functions.hpp"
#include "pinDefinitions.hpp"
#include "main.hpp"

void receivedData(UartClass *UART)
{
    delay(25);

    String receivedMessage;
    while (UART->available() > 0)
    {
        char letter = UART->read();
        receivedMessage += letter;
    }

    delay(25);

    if (receivedMessage.charAt(0) == '0' && receivedMessage.length() == 8)
    {
        unsigned long time = 0;
        time += ((receivedMessage.charAt(2) - '0') * 10000);
        time += ((receivedMessage.charAt(3) - '0') * 1000);
        time += ((receivedMessage.charAt(4) - '0') * 100);
        time += ((receivedMessage.charAt(5) - '0') * 10);
        time += (receivedMessage.charAt(6) - '0');

        motorMoveTimer(receivedMessage.charAt(1) - '0', time);
    }

    else if (receivedMessage.charAt(0) == '1' && receivedMessage.length() == 7)
    {
        long angle = 0;
        angle += ((receivedMessage.charAt(3) - '0') * 100);
        angle += ((receivedMessage.charAt(4) - '0') * 10);
        angle += (receivedMessage.charAt(5) - '0');

        if (receivedMessage.charAt(2) == '-')
            angle = -angle;

        motorMoveRelative(receivedMessage.charAt(1) - '0', angle);
    }

    else if (receivedMessage.charAt(0) == '2' && receivedMessage.length() == 6)
    {
        unsigned long angle = 0;
        angle += ((receivedMessage.charAt(2) - '0') * 100);
        angle += ((receivedMessage.charAt(3) - '0') * 10);
        angle += (receivedMessage.charAt(4) - '0');

        motorMoveAbsolute(receivedMessage.charAt(1) - '0', angle);
    }

    else if (receivedMessage.charAt(0) == '3' && receivedMessage.length() == 4)
    {
        if (receivedMessage.charAt(1) == '0')
            startMotorMove(receivedMessage.charAt(2) - '0');

        else if (receivedMessage.charAt(1) == '1')
            stopMotorMove(receivedMessage.charAt(2) - '0');

        else if (receivedMessage.charAt(1) == '2')
            calibrate();
    }

    else if (receivedMessage.charAt(0) == '4' && receivedMessage.length() == 3)
        missileLaunch(receivedMessage.charAt(1) - '0');

    else if (receivedMessage.charAt(0) == '5' && receivedMessage.length() == 3)
    {
        String messageToSend = "";

        if (receivedMessage.charAt(1) == '0')
        {
            if (baseAngle < 100)
                messageToSend += '0';

            if (baseAngle < 10)
                messageToSend += '0';

            messageToSend += int(baseAngle);
            messageToSend += '/';

            if (BASE_ANGLE < 100)
                messageToSend += '0';
            
            messageToSend += BASE_ANGLE;
            messageToSend += ',';

            if (angleAngle < 100)
                messageToSend += '0';

            if (angleAngle < 10)
                messageToSend += '0';

            messageToSend += int(angleAngle);
            messageToSend += '/';

            if (ANGLE_ANGLE)
                messageToSend += '0';
            
            messageToSend += ANGLE_ANGLE;
        }

        else if (receivedMessage.charAt(1) == '1')
        {
            if (digitalRead(PIN_MISSILE_1_SENSOR) == 0)
                messageToSend += '1';

            else
                messageToSend += '0';

            if (digitalRead(PIN_MISSILE_2_SENSOR) == 0)
                messageToSend += '1';

            else
                messageToSend += '0';

            if (digitalRead(PIN_MISSILE_3_SENSOR) == 0)
                messageToSend += '1';

            else
                messageToSend += '0';
        }

        UART->println(messageToSend);
    }
}