# mbn-nodemcu
MailBoxNotifica Remote Sensor

Have you ever looked forward to an online purchase that you spent every day anxiously opening your mailbox just to find it empty? Ever wish that the physical mailbox can notify you just like the electronic mail does? In conclusion, my IoT project aim is to transform an ordinary mailbox into something similar to an email that can send notification so that you will not need to visit it every now and then.

In the aspect of the design, I would need a sensor in the mailbox to detect if the mailbox is empty or not. If it detects the object in the box, the sensor will need to notify the owner through a medium. That medium will then need to forward the notification to the owner.

![image](https://user-images.githubusercontent.com/55645717/159127091-c3d0061a-4c42-480a-9c0f-d74d0d9fab59.png)

A MQTT server would be great to serve as the medium, however I choose not to use it because at the current stage of the project, the broker is redundant as the medium only forward the notification to the owner. As such I decided to create a web application to facilitate the process.

In the end, I drew out a use case diagram displaying the functionality of the web application

![image](https://user-images.githubusercontent.com/55645717/159127113-c806464f-df8c-4db3-8da2-bb9514fafb69.png)

I will explain the use cases below:
•	Send Notification – The main purpose of the application. It receives the alert from the mailbox device and forward it the user.
•	Log In – User will require to log in the application; thus, email address and number of devices will be tied to one user. It can act as a lite form of security and future proving the application.
•	Verify identity – User will need to verify their email address in order to prove that the notification is sent to the right person. The device would need to verify that it is the legit machine to use the such service.
•	Receive Notification – User will receive an email notification when the mailbox receives good of any kind
•	View History – In an event that the user disables the notification, he/she can still view the alert in the application.
•	Add Device – User can have one or more device to send out alerts. It can be used in home mailbox or work pigeonhole e.g.
•	Disable Notification – Turn the email alert off. An alert will still display in the history of the application

![image](https://user-images.githubusercontent.com/55645717/159127122-9db5c9a4-d364-470c-8b31-d19a718c0eec.png)

The above shows the process model of the application. When the mailbox is not empty, the device will send a message to the backend service. Depending on the option of the notification, email will be sent to the owner. Either way, the event will be recorded in user history. Accessing the frontend will show the historical event.

## Hardware Device - Concept
The main functionality of the hardware is to detect the area in the mailbox whether there is an item and it will send an alert to the web application to trigger an email notification to the owner. My starting point was to imitate how bat use echolocation to detect obstacle, however this would drive the overhead of the project higher as sound sensor is slightly costly. Furthermore, it might consume more energy to power the device. 

Alternatively, instead of using sound to detect the object, I moved to utilize light as my source. An LED and LDR is very cost effective and energy efficient. The concepts are as shown below:

![image](https://user-images.githubusercontent.com/55645717/159127169-06ef29ad-7035-457f-9693-8d0f82b6b443.png)

There will be two LDR sensor. One of the LDR sensor will reside near the entrance of the mailbox while another will be at the center bottom. An LED will be placed slightly off the top center of the mailbox.

The LDR sensors are connected in serial and to A0 pin of the NodeMCU. The NodeMCU will output certain value due to the amount of light detected by the LDR sensor. This can be triggered by flashing the LED for a mere second. Based on the value, we can tell whether the mailbox is empty or not. The following pages show the type of object interacting with the hardware.

![image](https://user-images.githubusercontent.com/55645717/159127203-77bc0a8b-92f8-4caf-8d1e-f5763f470133.png)
![image](https://user-images.githubusercontent.com/55645717/159127206-304149ee-295f-4464-b995-aefee1ebed20.png)
![image](https://user-images.githubusercontent.com/55645717/159127209-e6ca6575-d182-4adb-97a4-9011a11ce8d7.png)

The value return by the IDR sensor co-relate to the light intensity. This means that when both sensors detect light, it will output a higher value and low value when one of the sensors is blocked. This way, whenever the value falls below a defined threshold value, we can determine there is an object in the mailbox.

![image](https://user-images.githubusercontent.com/55645717/159127213-e125ed2b-c6b9-45d6-8d48-0dd37ffce011.png)

## Hardware Device – Circuit
•	NodeMCU ESP 8266
•	RED LED 5mm
•	LDR
•	Button	
•	10K Resister
