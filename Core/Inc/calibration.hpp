#pragma once

#ifndef HARDWARE_ID
#error "please specify hardware id"
#endif

#define CALIBRATION_MODE
#define IGNORE_CAPACITOR_ERROR

#if (HARDWARE_ID == -1)

// template for setting calibration values

#define VA_GAIN 1
#define VA_BIAS 0

#define VB_GAIN 1
#define VB_BIAS 0

#define I_REFEREE_GAIN 1
#define I_REFEREE_BIAS 0

#define I_A_CONVERT_GAIN 1
#define I_A_CONVERT_BIAS 0

#define I_B_CONVERT_GAIN 1
#define I_B_CONVERT_BIAS 0

// I_REFEREE should have (RefereeSystem -> DCDC_ASide) as positive
// I_ASide should have (DCDC_ASide -> DCDC_BSide) as positive
// I_BSide should have (DCDC_ASide -> DCDC_BSide) as positive

#elif (HARDWARE_ID == 101)

// todo: set calibration values

#define VA_GAIN 0.008171604f
#define VA_BIAS 0.016343207f

#define VB_GAIN 0.008097166f
#define VB_BIAS 0.042510121f

#define I_REFEREE_GAIN 0.020512821f
#define I_REFEREE_BIAS (-41.98717949f)

#define I_A_CONVERT_GAIN 0.020387755f
#define I_A_CONVERT_BIAS (-41.74912245f)

#define I_B_CONVERT_GAIN (-0.020253165f)
#define I_B_CONVERT_BIAS 41.43037975f

// I_REFEREE should have (REFEREESystem -> DCDC_ASide) as positive
// I_Chassis should have (Chassis -> DCDC_ASide) as positive

#elif (HARDWARE_ID == 102)

// todo: set calibration values

#define VA_GAIN 0.008121827f
#define VA_BIAS 0.060913706f

#define VB_GAIN 0.008182241f
#define VB_BIAS 0.051603905f

#define I_REFEREE_GAIN 0.020465473f
#define I_REFEREE_BIAS (-41.88907353f)

#define I_A_CONVERT_GAIN 0.020332276f
#define I_A_CONVERT_BIAS (-41.61545149f)

#define I_B_CONVERT_GAIN (-0.020356234f)
#define I_B_CONVERT_BIAS 41.61577608f

#elif (HARDWARE_ID == 201)

#define VA_GAIN 0.008076729f
#define VA_BIAS 0.037354871f

#define VB_GAIN 0.008121827f
#define VB_BIAS 0.040609137f

#define I_REFEREE_GAIN 0.020382166f
#define I_REFEREE_BIAS (-41.70063694f)

#define I_A_CONVERT_GAIN 0.020110831f
#define I_A_CONVERT_BIAS (-41.13824685f)

#define I_B_CONVERT_GAIN (-0.020253165f)
#define I_B_CONVERT_BIAS 41.41772152f

#elif (HARDWARE_ID == 202)

#define VA_GAIN 0.008080808f
#define VA_BIAS 0.016161616f

#define VB_GAIN 0.008106543f
#define VB_BIAS 0.034742328f

#define I_REFEREE_GAIN 0.020260492f
#define I_REFEREE_BIAS (-41.42981187f)

#define I_A_CONVERT_GAIN 0.020068807f
#define I_A_CONVERT_BIAS (-41.05332569f)

#define I_B_CONVERT_GAIN (-0.020097617f)
#define I_B_CONVERT_BIAS 41.09072639f

#elif (HARDWARE_ID == 203)

#define VA_GAIN 0.008078632f
#define VA_BIAS 0.018580854f

#define VB_GAIN 0.008072654f
#define VB_BIAS 0.039556004f

#define I_REFEREE_GAIN 0.020091848f
#define I_REFEREE_BIAS (-41.05912744f)

#define I_A_CONVERT_GAIN 0.020068807f
#define I_A_CONVERT_BIAS (-41.01920872f)

#define I_B_CONVERT_GAIN (-0.020063055f)
#define I_B_CONVERT_BIAS 41.00171969f

#elif (HARDWARE_ID == 204)

#define VA_GAIN 0.008101539f
#define VA_BIAS 0.025924926f

#define VB_GAIN 0.008090927f
#define VB_BIAS 0.035137738f

#define I_REFEREE_GAIN 0.020242915f
#define I_REFEREE_BIAS (-41.37044534f)

#define I_A_CONVERT_GAIN 0.02013809f
#define I_A_CONVERT_BIAS (-41.16340621f)

#define I_B_CONVERT_GAIN (-0.020178726f)
#define I_B_CONVERT_BIAS 41.25454021f

#elif (HARDWARE_ID == 205)


#define VA_GAIN 0.00807537f
#define VA_BIAS 0.02422611f

#define VB_GAIN 0.008096386f
#define VB_BIAS 0.041638554f

#define I_REFEREE_GAIN 0.020207852f
#define I_REFEREE_BIAS (-41.30051963f)

#define I_A_CONVERT_GAIN 0.020190366f
#define I_A_CONVERT_BIAS (-41.28785694f)

#define I_B_CONVERT_GAIN (-0.020219526f)
#define I_B_CONVERT_BIAS 41.32986713f

#endif

#define ADC_BUFFER_SIZE (ADC_CHANNEL_COUNT * ADC_SAMPLE_COUNT)

#define CALI_VA(vA_raw) (VA_GAIN * DIVIDED_BY_ADC_SAMPLE_COUNT * (vA_raw) + VA_BIAS)
#define CALI_VB(vB_raw) (VB_GAIN * DIVIDED_BY_ADC_SAMPLE_COUNT * (vB_raw) + VB_BIAS)
#define CALI_I_REFEREE(iReferee_raw) (I_REFEREE_GAIN * DIVIDED_BY_ADC_SAMPLE_COUNT * (iReferee_raw) + I_REFEREE_BIAS)
#define CALI_I_A_CONVERT(iAIn_raw) (I_A_CONVERT_GAIN * DIVIDED_BY_ADC_SAMPLE_COUNT * (iAIn_raw) + I_A_CONVERT_BIAS)
#define CALI_I_B_CONVERT(iCap_raw) (I_B_CONVERT_GAIN * DIVIDED_BY_ADC_SAMPLE_COUNT * (iCap_raw) + I_B_CONVERT_BIAS)
