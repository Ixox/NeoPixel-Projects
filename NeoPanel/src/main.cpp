/*
  Copyright © 2018, Xavier Hosxe
  Free license, do what you want with these files
*/

#include <string.h>

#include "main.h"
#include "dma.h"
#include "rng.h"
#include "usart.h"
#include "gpio.h"

#include "Strip.h"
#include "StripAnimation.h"

extern "C" {
void SystemClock_Config(void);
}

#define DMA_BUFFER_SIZE 64
char dmaTextReceived[DMA_BUFFER_SIZE+1];

// Clalled in DMA interupt for flashInfo
StripAnimation *stripAnimation;
char newMessage[DMA_BUFFER_SIZE+1];

int main(void)
{
  memset(dmaTextReceived, 0, DMA_BUFFER_SIZE +1);

  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RNG_Init();
  MX_USART1_UART_Init();

  Strip *strip = new Strip(NeoPin_GPIO_Port, NeoPin_Pin);
  stripAnimation = new StripAnimation(strip);
  stripAnimation->init();


  // Let's enable IDLE RX interupt

  // __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  // __HAL_DMA_ENABLE_IT (&hdma_usart1_rx, DMA_IT_TC); // enable DMA Rx cplt
  // __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
  
  char message[] = "\r\nstart\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*) message , 9, 20);

  dmaTextReceived[DMA_BUFFER_SIZE] = 0;
  HAL_UART_Receive_DMA(&huart1, (uint8_t*)dmaTextReceived, DMA_BUFFER_SIZE);
  newMessage[0] = 0;
  newMessage[DMA_BUFFER_SIZE] = 0;

  while (1)
  {
    HAL_GPIO_TogglePin(LedPin_GPIO_Port, LedPin_Pin);
    stripAnimation->loop2();

    if (newMessage[0] !=  0) {
        stripAnimation->setFlashInfo(newMessage);
        newMessage[0] = 0;
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    char message[] = "Received";
    HAL_UART_Transmit(&huart1, (uint8_t*) message , 8, 20);
    memcpy(newMessage, dmaTextReceived, 8);
    for (int c = 0; c < DMA_BUFFER_SIZE; c++) {
      if (dmaTextReceived[c] == '_') {
        newMessage[c] = 0;
        break;
      } else {
        newMessage[c] = dmaTextReceived[c];
      }
    }

  // // Force last char to 0 to end the string
  // dmaTextReceived[DMA_MAX_SIZE] = 0;
  // stripAnimation->setFlashInfo(dmaTextReceived);
  // char response[128];
  // sprintf(response, "%i chars\r\n", strlen(dmaTextReceived));
  // HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 20);

  // char response2[128];
  // sprintf(response2, "%s\r\n", dmaTextReceived);
  // HAL_UART_Transmit(&huart1, (uint8_t*)response2, strlen(response2), 20);

  // dmaTextReceived[0] = 0;
  // HAL_UART_Receive_DMA(&huart1, (uint8_t*)dmaTextReceived, DMA_MAX_SIZE);
}