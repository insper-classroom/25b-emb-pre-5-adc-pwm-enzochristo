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
 repeating_timer_t timerle;




 
 const float conversion_factor = 3.3f / (1 << 12);
 int volatile flag = 0;

 volatile int g_timer_0 = 0;
 volatile int g_timer_1 = 0;
 volatile int le =0;


 bool timer_0_callback(repeating_timer_t *rt) {
        g_timer_0 = 1;
        return true; // keep repeating
    }

 bool timer_le_callback(repeating_timer_t *rt) {
        le = 1;
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
    adc_gpio_init(28);
    adc_select_input(2);



    uint16_t result;
    float voltagem;
    int ja_comecou_timer0 = 0;
    int ja_comecou_timer1 = 0;

    gpio_init(PIN_LED_B);
    gpio_put(PIN_LED_B,0);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    add_repeating_timer_ms(10, timer_le_callback, NULL, &timerle);

    // gpio_init(PIN_ADC);
    // gpio_set_dir(PIN_ADC, GPIO_IN);
    // // o up e down sao referentes aos niveis de quando a tensao esta HIGH
    int led_status = 0;
    int voltagem = 0;

    while (1) {

        
        if (le){
            result = adc_read();
            voltagem = result * conversion_factor;
            printf("%f\n", voltagem);
            le = 0;
        }
        
        if((voltagem >= 1.0) && (voltagem <= 2.0) && (ja_comecou_timer0 == 0)){
            if(ja_comecou_timer1 == 1){
                cancel_repeating_timer(&timer1);
                ja_comecou_timer1 = 0;
            }

            printf("timer ativo \n");
            add_repeating_timer_ms(300, timer_0_callback, NULL, &timer0);
            // led_status = 1;
            gpio_put(PIN_LED_B,1);
            ja_comecou_timer0 = 1;
        }

        if((voltagem > 2.0) && (ja_comecou_timer1 == 0)){
            if(ja_comecou_timer0 == 1){
                cancel_repeating_timer(&timer0);
                ja_comecou_timer0 = 0;
            }

            printf("timer ativo \n");
            add_repeating_timer_ms(500, timer_1_callback, NULL, &timer1);
            // led_status = 1;
            gpio_put(PIN_LED_B,1);
            ja_comecou_timer1 = 1;
        }
        if (!voltagem){
            if((ja_comecou_timer0 == 1)){
                cancel_repeating_timer(&timer0);
                ja_comecou_timer0 = 0; 
            }
            else if( ja_comecou_timer1 == 1){
                cancel_repeating_timer(&timer1);
                ja_comecou_timer1 = 0; 
            }
            //     // g_timer_0 = 0;
            //     // g_timer_1 = 0;
            // }
            gpio_put(PIN_LED_B, 0);
        }

        if(g_timer_0 || g_timer_1){
            led_status = !led_status;
            gpio_put(PIN_LED_B,led_status);
            g_timer_0 = 0;
            g_timer_1 = 0;
        }
    }
    
}

