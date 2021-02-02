# Password-Based-Security-Door-Lock
First,keypad and LCD interface with MC1,user enters the password(Password of 5 digits from 0 to 9)and it appears on LCD.\
Second,when the password is entered correct twice,it will be sent using UART to MC2 and saved in the EEPROM.
Entering a new password will be done only once in the whole life, unless the user choosed to change it.
Third,the main options appear and the user chooses. Once he chooses something,he enters the password using keypad and appears on LCD.Then,it will be sent to MC2 using UART to check it and compare it with the one that is saved in the EEPROM.
If it is entered wrong for 3 times,MC1 will send a flag to MC2 using UART that indicates that the chances have been finished and MC2 will make the buzzer on for 1 min,then will return to the main options again.
If he entered it right for less than 3 times error,if he choosed:
1- *:It will return to the first step and will save a new password.
2-%:MC1 will send to MC2,so the motor rotates clockwise for 15 secs.Then,the motor stops for 3 secs.Then,the motor rotates anti-clockwise for 15 secs.
Then,the system returns to the main options.

Password Based Security Door Lock on 2 ATMEGA16 microcontrollers, one "HMI" for user interfacing using LCD and Keypad and the other "Controller" for storing data in the external M24C16 EEPROM controlling the DC motor and Buzzer which is used for alarm,2020. Drivers Designed and Implemented for project: Keypad, LCD, Timer, UART, I2C, External EEPROM
