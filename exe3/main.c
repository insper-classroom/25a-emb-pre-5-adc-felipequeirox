#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;

    const int window_size = 5;
    int window[5] = {0};
    int index = 0;
    int count = 0;
    int sum = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // Remove valor antigo da soma
            sum -= window[index];

            // Insere novo valor
            window[index] = data;

            // Adiciona novo valor à soma
            sum += data;

            // Atualiza índice circular
            index = (index + 1) % window_size;

            // Evita divisão por 0 no início
            if (count < window_size) {
                count += 1;
            }

            if (count == window_size) {
                float filtered_value = (float)sum / window_size;
                printf("Filtered value: %.2f\n", filtered_value);
            }
            
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
