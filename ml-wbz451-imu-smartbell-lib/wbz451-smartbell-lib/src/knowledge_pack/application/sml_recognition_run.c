#include "kb.h"
#include "kb_output.h"
#include <string.h>
#include "peripheral/sercom/usart/plib_sercom0_usart.h"
#ifdef SML_USE_TEST_DATA
#include "testdata.h"
int32_t td_index = 0;
#endif // SML_USE_TEST_DATA

#define SERIAL_OUT_CHARS_MAX 512

static char serial_out_buf[SERIAL_OUT_CHARS_MAX];

void sml_output_results(uint16_t model, uint16_t classification)
{
    memset(serial_out_buf, 0, SERIAL_OUT_CHARS_MAX);
    kb_sprint_model_result(model, serial_out_buf, false, false, true);
    printf("%s\n", serial_out_buf);
}

#define NUM_CLASSES 4u
/*
 * "1": "Bicepcurl",
 * "2": "lateral",
 * "3": "shoulder",
 * "0": "Unknown"
 */
static uint8_t class_map[NUM_CLASSES] = {0u};

int32_t sml_recognition_run(int16_t *data, int32_t num_sensors)
{
    int32_t ret;
    ret = kb_run_model((int16_t *)data, num_sensors, KB_MODEL_BASE_RANK_0_INDEX);
    if (ret >= 0){
        SERCOM0_USART_WriteByte(0xA5);
        class_map[0u] = 0u;//unknown//
        class_map[1u] = 0u;//Biceps//
        class_map[2u] = 0u;//lateral//
        class_map[3u] = 0u;//shoulder//
        switch(ret)
        {
          case 0u:
              class_map[0u]=1u;
              break;
          case 1u:
              class_map[1u]=1u;
              break;
          case 2u:
              class_map[2u]=1u;
              break;
          case 6u:
              class_map[3u]=1u;
              break;
        }
        SERCOM0_USART_Write(class_map,4u);
//        SERCOM5_USART_Write(&runtime,4u);
        SERCOM0_USART_WriteByte(~0xA5);
//        sml_output_results(KB_MODEL_BASE_RANK_0_INDEX, ret);
        kb_reset_model(0);
    };
        return ret;
}
