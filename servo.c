#include "servo.h"
#include "gpio_config.h"
#include "delay.h"

static int current_angle = 0;   // track last angle

void Servo_Init(void)
{
    TIM4->PSC = 16 - 1;        // 16 MHz / 16 = 1 MHz ? 1 µs tick
    TIM4->ARR = 20000 - 1;     // 20 ms period (50 Hz)

    TIM4->CCR1 = 600;         // 1 ms center pulse

    TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM4->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE;

    TIM4->CCER |= TIM_CCER_CC1E;  // Enable CH1 output
    TIM4->CR1  |= TIM_CR1_ARPE;
    TIM4->CR1  |= TIM_CR1_CEN;    // Start timer
}

void Servo_SetAngle(int angle)
{
    // Map 0–180° ? 600–2400 µs
    uint16_t pulse = (angle * (2400 - 600) / 180) + 600;
    TIM4->CCR1 = pulse;
    current_angle = angle;
}

// Sweep smoothly to target angle in given duration (ms)
void Servo_SweepTo(int target_angle, int duration_ms)
{
    int steps;
    int delay_per_step;
    int angle;
    int step_size;

    // Calculate difference
    if (target_angle > current_angle)
        steps = target_angle - current_angle;
    else
        steps = current_angle - target_angle;

    if (steps == 0) return;

    // Use larger increments (e.g., 3° per step)
    step_size = 3;
    steps = steps / step_size;
    if (steps == 0) steps = 1;

    delay_per_step = duration_ms / steps;

    if (target_angle > current_angle)
    {
        for (angle = current_angle; angle <= target_angle; angle += step_size)
        {
            Servo_SetAngle(angle);
            Delay_ms(delay_per_step);
        }
    }
    else
    {
        for (angle = current_angle; angle >= target_angle; angle -= step_size)
        {
            Servo_SetAngle(angle);
            Delay_ms(delay_per_step);
        }
    }

    // Ensure final position is exact
    Servo_SetAngle(target_angle);
}

void Servo_DoorUnlock(void)
{
    Servo_SweepTo(135, 2000);   // sweep to 135° in 2 seconds
}

void Servo_DoorLock(void)
{
    Servo_SweepTo(0, 2000);     // sweep back to 0° in 2 seconds
}