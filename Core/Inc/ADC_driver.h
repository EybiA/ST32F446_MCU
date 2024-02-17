

#include "main.h"

#ifndef INC_ADC_DRIVER_H_
#define INC_ADC_DRIVER_H_

#define TEMP_SENSOR_AVG_SLOPE_MV_PER_CELSIUS                        2.5f
#define TEMP_SENSOR_VOLTAGE_MV_AT_25                                760.0f
#define ADC_REFERENCE_VOLTAGE_MV                                    3300.0f
#define ADC_MAX_OUTPUT_VALUE                                        4095.0f

extern void MX_ADC1_Init(void);
extern float read_ADC(void);
extern void MX_DAC_Init(void);
extern void DAC_output(void);
extern void DAC_output_Stop(void);

#endif /* INC_ADC_DRIVER_H_ */
