/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdint.h>
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include "ringbuffer.h"
#include "sensor.h"
#include "app_config.h"
#include "sml_recognition_run.h"

// *****************************************************************************
// *****************************************************************************
// Section: Platform specific includes
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global variables
// *****************************************************************************
// *****************************************************************************

static volatile uint32_t tickcounter = 0;
static volatile unsigned int tickrate = 0;

static struct sensor_device_t sensor;
static snsr_data_t _snsr_buffer_data[SNSR_BUF_LEN][SNSR_NUM_AXES];
static ringbuffer_t snsr_buffer;
static volatile bool snsr_buffer_overrun = false;

// *****************************************************************************
// *****************************************************************************
// Section: Platform specific stub definitions
// *****************************************************************************
// *****************************************************************************

size_t __attribute__(( unused )) UART_Write(uint8_t *ptr, const size_t nbytes) {
    return SERCOM0_USART_Write(ptr, nbytes) ? nbytes : 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Generic stub definitions
// *****************************************************************************
// *****************************************************************************
void Null_Handler() {
    // Do nothing
}

static void Ticker_Callback() {
    static uint32_t mstick = 0;

    //GPIO_PortToggle(GPIO_PORT_A,0x00000200);
    
    ++tickcounter;
    if (tickrate == 0 || mstick > tickrate) {
        mstick = 0;
    }
    else if (++mstick == tickrate) {
        LED_STATUS_Toggle();
        mstick = 0;
    }
}

uint64_t read_timer_ms(void) {
    return tickcounter;
}

uint64_t read_timer_us(void) {
    return tickcounter * 1000U + (uint32_t) TC_TimerGet_us();
}

void sleep_ms(uint32_t ms) {
    uint32_t t0 = read_timer_ms();
    while ((read_timer_ms() - t0) < ms) { };
}

void sleep_us(uint32_t us) {
    uint32_t t0 = read_timer_us();
    while ((read_timer_us() - t0) < us) { };
}

// For handling read of the sensor data
void SNSR_ISR_HANDLER() {
    /* Check if any errors we've flagged have been acknowledged */
    if ((sensor.status != SNSR_STATUS_OK) || snsr_buffer_overrun)
        return;
    
    ringbuffer_size_t wrcnt;
    snsr_data_t *ptr = ringbuffer_get_write_buffer(&snsr_buffer, &wrcnt);
    
    if (wrcnt == 0)
        snsr_buffer_overrun = true;
    else if ((sensor.status = sensor_read(&sensor, ptr)) == SNSR_STATUS_OK)
        ringbuffer_advance_write_index(&snsr_buffer, 1);
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    int8_t app_failed = 0;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    kb_model_init();
    
    /* Register and start the millisecond interrupt ticker */
    TC_TimerCallbackRegister(Ticker_Callback);
    TC_TimerStart();

    printf("\n");

    MIKRO_INT_Disable();
    MIKRO_INT_CallbackRegister(Null_Handler);
    
    /* Application init routine */
    app_failed = 1;
    while (1)
    {
        /* Initialize the sensor data buffer */
        if (ringbuffer_init(&snsr_buffer, _snsr_buffer_data, sizeof(_snsr_buffer_data) / sizeof(_snsr_buffer_data[0]), sizeof(_snsr_buffer_data[0])))
            break;

        /* Init and configure sensor */
        if (sensor_init(&sensor) != SNSR_STATUS_OK) {
            printf("ERROR: sensor init result = %d\n", sensor.status);
            break;
        }

        if (sensor_set_config(&sensor) != SNSR_STATUS_OK) {
            printf("ERROR: sensor configuration result = %d\n", sensor.status);
            break;
        }

        printf("sensor type is %s\n", SNSR_NAME);
        printf("sensor sample rate set at %dHz\n", SNSR_SAMPLE_RATE);
#if SNSR_USE_ACCEL
        printf("accelerometer enabled with range set at +/-%dGs\n", SNSR_ACCEL_RANGE);
#else
        printf("accelerometer disabled\n");
#endif
#if SNSR_USE_GYRO
        printf("gyrometer enabled with range set at %dDPS\n", SNSR_GYRO_RANGE);
#else
        printf("gyrometer disabled\n");
#endif
        
        /* Activate External Interrupt Controller for sensor capture */
        
        MIKRO_INT_CallbackRegister(SNSR_ISR_HANDLER);
        MIKRO_INT_Enable();
        
        /* STATE CHANGE - Application successfully initialized */
        tickrate = 0;
        LED_ALL_Off();
        LED_STATUS_On();

        /* STATE CHANGE - Application is streaming */
        tickrate = TICK_RATE_SLOW;

        app_failed = 0;
        break;
    }

    while (!app_failed)
    {
        /* Maintain state machines of all system modules. */
        SYS_Tasks ( );

        if (sensor.status != SNSR_STATUS_OK) {
            printf("ERROR: Got a bad sensor status: %d\n", sensor.status);
            break;
        }

        else if (snsr_buffer_overrun == true) {
            printf("\n\n\nOverrun!\n\n\n");

            /* STATE CHANGE - buffer overflow */
            tickrate = 0;
            LED_ALL_Off();
            LED_STATUS_On(); LED_RED_On();
            sleep_ms(5000U);

            // Clear OVERFLOW
            MIKRO_INT_CallbackRegister(Null_Handler);
            ringbuffer_reset(&snsr_buffer);
            snsr_buffer_overrun = false;
            MIKRO_INT_CallbackRegister(SNSR_ISR_HANDLER);

            /* STATE CHANGE - Application is streaming */
            tickrate = TICK_RATE_SLOW;
            LED_ALL_Off();
            continue;
        }
        else if(ringbuffer_get_read_items(&snsr_buffer) >= SNSR_SAMPLES_PER_PACKET) {
            ringbuffer_size_t rdcnt;
            snsr_dataframe_t const *ptr = ringbuffer_get_read_buffer(&snsr_buffer, &rdcnt);
            while (rdcnt >= SNSR_SAMPLES_PER_PACKET) {
    #if STREAM_FORMAT_IS(ASCII)
                snsr_data_t const *scalarptr = (snsr_data_t const *) ptr;
                printf("%d", *scalarptr++);
                for (int j=1; j < sizeof(snsr_datapacket_t) / sizeof(snsr_data_t); j++) {
                    printf(" %d", *scalarptr++);
                }
                printf("\n");
    #elif STREAM_FORMAT_IS(MDV)
                uint8_t headerbyte = MDV_START_OF_FRAME;
                UART_Write(&headerbyte, 1);
                UART_Write((uint8_t *) ptr, sizeof(snsr_datapacket_t));
                headerbyte = ~headerbyte;
                UART_Write(&headerbyte, 1);
                ptr += SNSR_SAMPLES_PER_PACKET;
    #elif STREAM_FORMAT_IS(INFERENCE)
                for (int j=0; j < sizeof(snsr_datapacket_t) / sizeof(snsr_data_t); j++) {
                    sml_recognition_run((snsr_data_t *)ptr++, SNSR_NUM_AXES);
                }
    #endif //STREAM_FORMAT_IS(ASCII)
                rdcnt -= SNSR_SAMPLES_PER_PACKET;
                ringbuffer_advance_read_index(&snsr_buffer, SNSR_SAMPLES_PER_PACKET);
            }
        }
    }

    tickrate = 0;
    LED_ALL_Off();
    LED_RED_On();
    
    /* Loop forever on error */
    while (1) {};

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/
