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


// int filtra(int *lista, int n){
//     int k = 1;
//     float media = 0;
//     for (int i = 0 ; i < n ; i++){
//         if((i+(k-1))%5==0&& i!=0){
//             media = media/5;
//             printf("media movel %d: %f", k-1, media);
//             k++;
//             i = k-1;
//             media = 0;
//         }

//         media += lista[i]; 
//     }
// }


// void filtra(int *lista, int n){
   
//     for (int i = 0; i <= n - 5; i++){
//         float media = 0;

//         for (int k = i; k <i + 5; k++ ){
//             media += lista[k];
//         }
//         media = media/5;
//         printf("media %d: %f", i, media);
//     }
// }

int calcula_index (int index){

    if (index < 5){
        return index;
    }
    return index%5;
    
}

void media_movel(int n, int index){
    static float buffer[5];
    static int contador = 0;
    static float soma = 0;

    index = calcula_index(index);

    if (contador >=5){
        printf("media movel %d :%f\n ", (contador-5), (soma/5));
        soma = soma - buffer[index];
        buffer[index] = n;

    }
    else{
        buffer[index] = n;
    }
    contador ++;
    soma+=buffer[index];
}

void process_task(void *p) {
    int data = 0;
    // int lista[64];
    int contador = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) { // recebe sempre de um em um dado, mas eh muito rapido.
            // implementar filtro aqui!
            // lista[contador] = data;
            media_movel(data, contador);
            contador++;

            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        // else{ // como nao chega mais dados, obrigatoriamente, agora a lista esta completa.
        //     filtra(lista, contador);
        
        // }

    }
}


int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int)); // tamanho do buffer, tamanho de cada numero 

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
