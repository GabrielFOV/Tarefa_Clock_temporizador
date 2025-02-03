#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs e botão.
#define BUTTON_PIN 5
#define LED_RED 11
#define LED_GREEN 12
#define LED_BLUE 13

// Definição do stado do sistema.
volatile bool aguardando_reset = false;

// Função para desligar o LED azul (último).
int apagar_led_blue(alarm_id_t id, void *user_data) {
    gpio_put(LED_BLUE, 0);
    aguardando_reset = false;  // Permite nova ativação pelo botão.
    printf("Aguardando pressionamento do botão...\n");
    return 0;
}

// Função para desligar o LED verde.
int apagar_led_green(alarm_id_t id, void *user_data) {
    gpio_put(LED_GREEN, 0);
    // Programa o desligamento do LED azul após mais 3s.
    add_alarm_in_ms(3000, &apagar_led_blue, NULL, false);
    return 0;
}

// Função para desligar o LED vermelho.
int apagar_led_red(alarm_id_t id, void *user_data) {
    gpio_put(LED_RED, 0);
    // Programa o desligamento do LED verde após mais 3s.
    add_alarm_in_ms(3000, &apagar_led_green, NULL, false);
    return 0;
}

//  Função de interrupção do botão.
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (!aguardando_reset) {
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        gpio_put(LED_BLUE, 1);
        aguardando_reset = true;

        // Inicia os timers para apagar os LEDs na sequência correta.
        add_alarm_in_ms(3000, &apagar_led_red, NULL, false);
    }
}

int main() {
    stdio_init_all();
    int tempo = 1;

    // Inicialização do led vermelho.
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

     // Inicialização do led verde.
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

     // Inicialização do led azul.
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    // Inicialização do botão.
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    //Interrupção provocada pelo botão.
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop principal para rodar infinitamente.
    while (true) {
        printf("Se passou %d segundo(s)\n", tempo);
        sleep_ms(1000);
        tempo++;
    }
}

