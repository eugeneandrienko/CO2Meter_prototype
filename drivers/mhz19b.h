/**
 * @brief Driver for MH-Z19B CO2 sensor.
 */


#ifndef MHZ19B_H
#define MHZ19B_H

#define MHZ19B_PREVIOUS_COMMAND_NOT_SENT 1

#define RESPONSE_NOT_RECEIVED -1
#define RESPONSE_WRONG_SENSOR_NO -2
#define RESPONSE_WRONG_CHECKSUM -3

void init_mhz19b(void);
int request_co2_concentration(void);
int read_co2_concentration(void);

#endif // MHZ19B_H
