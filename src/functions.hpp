#ifndef FUNCTIONS_DEFINITIONS
#define FUNCTIONS_DEFINITIONS

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define STILL 4

#define BASE 0
#define ANGLE 1

#define BASE_ANGLE 180
#define ANGLE_ANGLE 40

#define MOTOR_POSITION_UPDATE_FREQUENCY 10

// Temps d'attende lors de la réception d'un message : pour un message de 20 caractères, on a 20 x 8 = 160 bits. Le baud rate est de 115200, donc 160 / 115200 = 1,39 ms. On ajoute 10 ms pour la sécurité.
#define UART_WAITING_TIME 160 / BAUD_RATE + 10

extern unsigned long upCounter;
extern unsigned long downCounter;
extern unsigned long leftCounter;
extern unsigned long rightCounter;
extern int availableMissiles;
extern int missileToLaunch;
extern unsigned long launcherCounter;
extern unsigned long motorPositionCounter;

void motorMoveTimer(int move, unsigned long time);
void motorMoveRelative(int direction, int angle);
void motorMoveAbsolute(int direction, int angle);
void startMotorMove(int move);
void stopMotorMove(int axis);
int checkAvailableMissiles();
void missileLaunch(int number);
boolean checkLeft();
boolean checkRight();
void calibrate();
void start();

#endif