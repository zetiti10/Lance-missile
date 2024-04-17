#ifndef MAIN_DEFINITIONS
#define MAIN_DEFINITIONS

#define BASE_ANGULAR_FREQUENCY_STORAGE_LOCATION 0
#define ANGLE_ANGULAR_FREQUENCY_STORAGE_LOCATION 1

#define BAUD_RATE 115200

extern double baseAngularFrequency;
extern double angleAngularFrequency;
extern double baseAngle;
extern double angleAngle;
extern double lastBaseAngleUpdateSended;
extern double lastAngleAngleUpdateSended;
extern String lastMissilesStateUpdateSended;
extern unsigned long updateInterval;
extern unsigned long lastUpdateSendedTime;

#endif