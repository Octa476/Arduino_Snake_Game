# Snake Game for Arduino

This project implements a classic Snake game on an Arduino with an LCD display. Use the buttons to control the snake and try to collect as many fruits as possible without hitting the walls or yourself!

## Hardware Requirements

- Arduino board (e.g., Uno, Mega)
- LCD display compatible with the `LCD_Driver` library
- 5 push buttons
- SPI interface

## Wiring

- **Button Connections:**
  - Right: Pin 2
  - Left: Pin 3
  - Up: Pin 4
  - Down: Pin 5
  - Start/Stop: Pin 6
- **LCD Connections:**
  - Use SPI connections as per your LCD's specifications.

## Libraries Used

- `SPI.h`: For communication with the LCD.
- `LCD_Driver.h`: Driver for the LCD display.
- `GUI_Paint.h`: For drawing graphics on the LCD.
- `EEPROM.h`: To save the highest score.

## Installation

1. Clone the repository or download the ZIP file.
2. Install the required libraries (`SPI`, `LCD_Driver`, `GUI_Paint`, `EEPROM`) in your Arduino IDE.
3. Connect the hardware as described in the **Wiring** section.

## Usage

1. Upload the code to your Arduino board.
2. Start the game by pressing the **Start/Stop** button.
3. Use the **Up**, **Down**, **Left**, and **Right** buttons to control the snake.
4. Avoid walls and the snake's body while collecting fruits to increase your score.

### Resetting the High Score

To reset the high score stored in EEPROM, uncomment the line `EEPROM.write(0, 0);` in the `setup()` function, upload the code, and run it once. Then comment it out again to preserve future high scores.

## Game Modes

- **Speed Settings:** Choose from Easy, Medium, Hard, or Expert.
- **Map Selection:** Play on different maps - Empty, Border, or Cross.
- **Custom Colors:** Customize snake and map colors.

## Contributing

Feel free to fork this repository and submit pull requests for improvements or bug fixes.

## License

This project is open-source and available under the MIT License.

