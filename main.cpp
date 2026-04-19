#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn enterButton(BUTTON1);
    DigitalIn Button0(D2);
    DigitalIn Button1(D3);
    DigitalIn Button2(D4);
    DigitalIn Button3(D5);
    DigitalIn Button4(D6);
    DigitalIn Button5(D7);

    DigitalOut warningLed(LED3);
    DigitalOut lockdownLed(LED2);
    DigitalOut activeAndBlinkLed(LED1);

    Button0.mode(PullDown);
    Button1.mode(PullDown);
    Button2.mode(PullDown);
    Button3.mode(PullDown);
    Button4.mode(PullDown);
    Button5.mode(PullDown);

    int incorrectCodeCount = 0;
    int lockdownTriggerCount = 0;
    int timerCounter = 0;
    int lockdownTotal = 0;
    bool lockdownCountLed = false;
    bool warningCleared = false;


    while (true) {

        //WARNING ALARM STATE
        if ( incorrectCodeCount == 3 ) {
            activeAndBlinkLed = OFF;
            timerCounter++;
            // make LED slow blink for 30 seconds
            warningLed = (timerCounter / 50) % 2;

            if ( timerCounter >= 3000 ) {
                incorrectCodeCount = 0;
                timerCounter = 0;
                warningLed = OFF;
                warningCleared = true;
            }
        }

        //LOCKDOWN ALARM STATE
        else if (incorrectCodeCount >= 4) {
            timerCounter++;
            lockdownLed = ON;
            activeAndBlinkLed = (timerCounter / 20) % 2;

            if (enterButton && Button0 && Button1 && Button4 && Button5) {
                incorrectCodeCount = 0;
                timerCounter = 0;
                lockdownLed = OFF;
                lockdownCountLed = true;
            }

            if (timerCounter >= 6000) {
                incorrectCodeCount = 0;
                timerCounter = 0;
                lockdownLed = OFF;
                lockdownCountLed = true;
            }
        }

        // NORMAL ALARM STATE
        else {
            activeAndBlinkLed = ON;
            if (enterButton) {
                // if correct code inputted:
                if ( Button2 && Button3 && Button4 && Button5 && !Button0 && !Button1 ) {
                    incorrectCodeCount = 0;
                    warningCleared = false;
                    activeAndBlinkLed = OFF; thread_sleep_for(200); activeAndBlinkLed = ON;
                } else {
                    // if incorrect blink warning led once
                    warningLed = ON;
                    thread_sleep_for(500);
                    warningLed = OFF;
                    if (warningCleared == true) {
                        incorrectCodeCount = 4;
                        warningCleared = false;
                    } else {
                        incorrectCodeCount++;
                    }
                    timerCounter = 0;

                    if (incorrectCodeCount >= 4) {
                        lockdownTotal++;
                    }
                }
                while(enterButton) {thread_sleep_for(10);}
            }
        }

        if (lockdownCountLed) {
            activeAndBlinkLed = OFF;
            thread_sleep_for(1000);
            for (int i =0; i < lockdownTotal; i++) {
                lockdownLed = warningLed = activeAndBlinkLed = ON;
                thread_sleep_for(500);
                lockdownLed = warningLed = activeAndBlinkLed = OFF;
                thread_sleep_for(500);
            }
            lockdownCountLed = false;  //reset it
        }
        thread_sleep_for(10);
    }
}
