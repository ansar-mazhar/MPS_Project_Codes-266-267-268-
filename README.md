This **README.md** is tailored to showcase the "Complex Engineering" aspects of your project, highlighting the hardware-software integration and the technical rigor required by the **EE-273** course.

---

# Secure Digital PIN Lock System (STM32F407)

A high-security embedded solution featuring non-volatile PIN storage, real-time user interaction, and automated physical actuation. This project was developed as a **Complex Engineering Problem (CEP)** at the University of Engineering and Technology (UET) Lahore.

## 🚀 Key Features

* 
**Security & Persistence:** Utilizes STM32 internal Flash (Sector 11) for non-volatile storage, ensuring the PIN remains saved during power cycles.


* 
**Precise Actuation:** High-torque door control via SG90 Servo using **PWM (TIM4)**.


* 
**User Interface:** 8-bit parallel LCD interface for status prompts and a 4x4 matrix keypad for secure entry.


* 
**Safety Logic:** Integrated 10-second automatic re-lock timer and manual override for emergency locking.


* **Visual Diagnostics:** Tri-color LED system (Red, Green, Yellow) for real-time system state indication.

## 🛠 Hardware Configuration

The system is built on the **STM32F407** microcontroller.

| Component | Port / Pins | Interface |
| --- | --- | --- |
| **LCD Data** | `PD0 - PD7` | 8-bit Parallel |
| **LCD Control** | `PB0 (RS), PB1 (RW), PB2 (EN)` | Digital Output |
| **Keypad** | `PC0-PC3 (Rows), PC4-PC7 (Cols)` | Matrix Scan |
| **Servo Motor** | `PB6` | PWM (AF2) |
| **LED Indicators** | `PE0 (G), PE1 (R), PE2 (Y)` | GPIO |

## 💻 Firmware Architecture

The firmware is designed with modularity and low-level register access to maximize performance and meet "Depth of Knowledge" requirements:

* **`gpio_config.c`**: Manual clock gating (RCC) and port mode configuration.
* **`pinsystem.c`**: Implements the main state machine, Flash memory R/W operations, and PIN comparison logic.
* **`lcd.c`**: Custom driver for 8-bit data transfer and text centering algorithms.
* **`delay.c`**: Precise timing based on the **ARM SysTick** timer (16MHz HSI clock).
* **`keypad.c`**: Non-blocking scanning logic with software debouncing.

## 🔧 Installation & Deployment

1. **Clone the Repo:** `git clone [repository-link]`
2. 
**Environment:** Set up **STM32CubeIDE** or Keil uVision for the STM32F4 series.


3. **Build:** Compile the source files ensuring the include paths for `stm32f4xx.h` are correctly mapped.
4. **Flash:** Connect your STM32F407 board via ST-Link and upload the `.bin`/`.elf` file.

## 📊 Measurement & Validation

As per course requirements, the system was validated against specific performance criteria:

* **Timing Accuracy:** SysTick-based delays validated via oscilloscope.
* **Flash Integrity:** Verified PIN persistence across 50+ power-cycle iterations.
* 
**Power Stability:** Servo isolated via external 5V source to prevent MCU brownouts.



---

**Next Step:** I can now generate the **Requirements Traceability Matrix** or the **Design Justification (including Power and Timing analysis)** to complete your report deliverables. Which should we do first?
