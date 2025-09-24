#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"

const int PWM_0_PIN = 4;
const int PWM_1_PIN = 6;

void led_task(void *p) {
    gpio_set_function(PWM_0_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_0_PIN);
    pwm_set_clkdiv(slice_num, 125);
    pwm_set_wrap(slice_num, 100);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 80);
    pwm_set_enabled(slice_num, true);

    while (true) {
    }
}


void ledg_task(void *p){
    // estamos falando para o hardware que esse pino vai ter a funcao especifica de PWM. O gpio pode ter diversas funcoes, essa vai ser a desse pino
    gpio_set_function(PWM_1_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_1_PIN); // Aqui estamos configurando o local interno que queremos que nosso output saia, existem varios setores de pwm dentro desse bloco que atendem a diferentes gpios.
    //  Precisamos encontrar qual eh referente ao nosso.

    uint canal_PWM = pwm_gpio_to_channel(PWM_1_PIN); // acha se eh o canal A ou B referente ao nosso pino


    // O primeiro argumento eh onde vamos aplicar a divisao e o segundo e por quanto vamos dividir.
    // Vamos pensar que nem sempre um valor de frequencia do clock que eh mto alto 125MHz eh ideal para nossos componentes.
    // Assim, precisamos diminuir esse tmpo de oscilacao. Por isso, usamos essa funcao. 
    // Ela eh resposnsavel por setar o periodo do nosso PWM para o slice tal, que eh a "regiao" dentro do nosso uc que eh referente a fazer o PWM ao nosso pino.
    pwm_set_clkdiv(slice_num, 125); 

    // Aqui determinamos o numero de ciclos que nosso PWM vai fazer.
    // O PWM eh um contador que vai contando a cada vez que o clock(com delay) faz "tick".
    // Esse wrap vai defifir quantos ticks o nosso clock vai fazer ate recomecar a contar o pwm
    // Ou seja vai setar quantos ticks o clock deve fazer para que o novo ciclo comece.
    // Assim estamos setando a frequencia do PWM, uma vez que vai recomecar o ciclo a cada 1/x ticks de clock por segundo
    // Assim a frequencia do pwm vai ser: f = 1/x*tempo de um tick do PWM; x eh o numero de ciclos.

    // pensar nas frequencias: Pensar em um low high low que o pwm faz e pensar no todo, ou seja todos os ciclos que o pwm faz.
    pwm_set_wrap(slice_num, 100);

    // define o dutycycle: A proporcao do tempo que vai ficar em high dentro do ciclo TOTAL DO PWM.
    // Ou seja, dentro dos, que seja, 100 ciclos, 90 ela vai ficar em high
    pwm_set_chan_level(slice_num, canal_PWM, 20);

    // Ativa o PWM.
    pwm_set_enabled(slice_num, true);



}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    xTaskCreate(led_task, "LED_Task 1", 256, NULL, 1, NULL);
    xTaskCreate(ledg_task, "LED_Task 2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
