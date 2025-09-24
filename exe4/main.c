/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/timer.h"
 #include "hardware/adc.h"
 
 const int PIN_LED_B = 4;
 const int PIN_ADC  = 28;
 repeating_timer_t timer0;
 repeating_timer_t timer1;


 
 const float conversion_factor = 3.3f / (1 << 12);
 int volatile flag = 0;

 volatile int g_timer_0 = 0;
 volatile int g_timer_1 = 0;


 bool timer_0_callback(repeating_timer_t *rt) {
        g_timer_0 = 1;
        return true; // keep repeating
    }

 bool timer_1_callback(repeating_timer_t *rt) {
        g_timer_1 = 1;
        return true; // keep repeating
    }

 /**
  * 0..1.0V: Desligado
  * 1..2.0V: 150 ms
  * 2..3.3V: 400 ms
  */

 int main() {
    stdio_init_all();
    adc_init();

    uint16_t result;
    float voltagem;
    int ja_comecou_timer0 = 0;
    int ja_comecou_timer1 = 0;


    gpio_init(PIN_LED_B);
    gpio_put(PIN_LED_B,0);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    


    gpio_init(PIN_ADC);
    gpio_set_dir(PIN_ADC, GPIO_IN);
    gpio_pull_up(PIN_ADC); // nivel high quando nao presisonado
    // o up e down sao referentes aos niveis de quando a tensao esta HIGH


    while (1) {
   
        adc_select_input(2);
        result = adc_read();
        voltagem = result * conversion_factor;

        // verificar a voltagem e iniciar o timer.
        printf("voltagem : %f\n", voltagem);

        if (voltagem > 0){

            if(voltagem < 2){
                if(ja_comecou_timer0){
                    add_repeating_timer_ms(300, timer_0_callback, NULL, &timer0);
                    gpio_put(PIN_LED_B,1);
                    ja_comecou_timer0 = 1;
                }

            }
            else{
                if (ja_comecou_timer1){
                    add_repeating_timer_ms(500, timer_0_callback, NULL, &timer1);
                    gpio_put(PIN_LED_B,1);
                    ja_comecou_timer1 = 1;
                }
            }

        }
        else{
            gpio_put(PIN_LED_B, 0);
        }

        // vai osclilar entre ligar e desligar dado o timer passar de tempo. 
        if(g_timer_0 && ja_comecou_timer0){
            gpio_put(PIN_LED_B,0);
            cancel_repeating_timer(&timer0);
            ja_comecou_timer0 = 0;
        }

        if(g_timer_1 && ja_comecou_timer1){
            gpio_put(PIN_LED_B,0);
            cancel_repeating_timer(&timer1);
            ja_comecou_timer1 = 0;
        }




       

    }



    }

