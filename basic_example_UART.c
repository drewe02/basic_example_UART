/*
 * This is a simple program to introduce UART.
 * Pushing S2 on the BoosterPack toggles the LED1 on the Launchpad at any time.
 * The program gets characters sent from your Laptop and decides whether it is Number, Letter or Other. Then,
 * it sends back N, L or O to the laptop in response.
 * Sending lower case c from laptop changes the baudrate. Currenlty, the only other baudrate implemented is 19200.
 * The system clock for this project is 3MHz and all the UART calcuations are based on this number.
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef enum {baud9600, baud19200, baud38400, baud57600} UARTBaudRate_t;

void TurnOn_Launchpad_LED1();
void TurnOff_Launchpad_LED1();
char S1isPressed();
void initializeGPIO();


 int main(void) {

     WDT_A_hold(WDT_A_BASE);
     initializeGPIO();

     eUSCI_UART_ConfigV1 uartConfig = {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 3MHz
          19,                                           // UCBR   = 19
          8,                                            // UCBRF  = 8
          0x55,                                         // UCBRS  = 0x55
          EUSCI_A_UART_NO_PARITY,                       // No Parity
          EUSCI_A_UART_LSB_FIRST,                       // LSB First
          EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
          EUSCI_A_UART_MODE,                            // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,// Oversampling
          EUSCI_A_UART_8_BIT_LEN                        // Data length is 8 bits
     };

     // initialize and enable EUSCI_A0
     UART_initModule(EUSCI_A0_BASE, &uartConfig);
     UART_enableModule(EUSCI_A0_BASE);

     // make sure Tx and Rx pins of EUSCI_A0 work for UART and not a regular GPIO pin
     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
         GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

     UARTBaudRate_t baudRate = baud9600;
     char rChar, tChar;

     while (1)
     {
         // To avoid blocking, we check to see if there is a character to process. In that case, we proceed to receiving it.
         if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                 == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
         { //beginning of "got new char"

             // calling this function without the above check results in a blocking code.
             // this function waits until it receives a character from UART
             rChar = UART_receiveData(EUSCI_A0_BASE);

             // Depending on if the received char is a Number, a Letter, or Otherwise, the transmit char is N, L or O
             if (('0'<=rChar) && (rChar <= '9'))
                 tChar = 'N';
             else if ((('a'<=rChar) && (rChar <= 'z')) ||
                      (('A'<=rChar) && (rChar <= 'Z')))
                 tChar = 'L';
             else
                 tChar = 'O';


             if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                     == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                 UART_transmitData(EUSCI_A0_BASE, tChar);

             // If the character is 'c', it also means to change the baudrarte
             if (rChar == 'c')
             {
                 switch(baudRate)
                 {
                 case baud9600:
                     uartConfig.clockPrescalar = 9;
                     uartConfig.firstModReg = 12;
                     uartConfig.secondModReg = 0x22;
                     UART_initModule(EUSCI_A0_BASE, &uartConfig);
                     UART_enableModule(EUSCI_A0_BASE);
                     baudRate = baud19200;
                     break;

                 case baud19200:
                     // to complete
                     break;

                 case baud38400:
                     // to complete
                     break;
                 } // end of switch-case

             } // end of if (rChar == 'c')
         } // end "got new char"

         // The non-blocking test. This part of code should always work smoothly if other parts of code are not blocking it.
         // If the button is not pressed, keep the LED off
         if (!S1isPressed())
             TurnOff_Launchpad_LED1();

         // otherwise, turn the LED on
         else
             TurnOn_Launchpad_LED1();

     } // end of while(1)

 }


//// Simple GPIO HAL for non-blocking test ///////////////////////////////////

 // According to the schematics on page 37 of the Launchpad user guide,
 // When a button is pressed, it is grounded (logic 0)
 #define PRESSED 0

 // The HAL itself is written using Driverlib, so it is much easier to implement.
 void initializeGPIO()
 {


     // Initializing LED1, which is on Pin 0 of Port P1 (from page 37 of the Launchpad User Guide)
     GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

     // Initializing S1 (switch 1 or button 1),
     // which is on Pin1 of Port 1 (from page 37 of the Launchpad User Guide)
     GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);}


 void TurnOn_Launchpad_LED1()
 {
     GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
 }

 void TurnOff_Launchpad_LED1()
 {
     GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
 }

 char S1isPressed()
 {
     return ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == PRESSED));
 }
