
 ![logo_microchip](Images/microchip.png)  ![logo_ml](Images/MPLAB-MachineLearning.png)
# ml-PIC33CX-BZ2-and-WBZ451-IMU-SmartBell

| ![SmartBell](Images/1.PNG) |
| :----------------------------------------------------------: |
|                        |

## Repository Overview
This repository is a companion to the [ML applications with MPLAB ML tutorial](https://onlinedocs.microchip.com/oxy/GUID-80D4088D-19D0-41E9-BE8D-7AE3BE021BBF-en-US-3/GUID-E6CBB10A-FFC8-4EF3-8C07-D29B64446EB6.html). It contains the firmware to Transform your conventional dumbbell into an intelligent training device utilizing the PIC32CX-BZ2 / WBZ451 Machine Learning Evaluation Kit in conjunction with the MPLAB X Machine Learning Development Suite on [PIC33CX-BZ2 and WBZ451 Curiosity Development Board](https://www.microchip.com/en-us/development-tool/ev96b94a) with the [Mikroe IMU Click board](https://www.mikroe.com/6dof-imu-2-click)

The ML Development Suite streamlines the development of machine learning solutions specifically tailored for application and deployment on Microchip’s microcontrollers and microprocessors. This manual offers a comprehensive walkthrough, demonstrating the steps to gather 6-axis IMU data from the PIC32CX-BZ2 / WBZ451 Machine Learning Evaluation Kit( the logger firmware can be found on the latest GitHub [ml-PIC33CX-BZ2-and-WBZ451-IMU-Datalogger](https://github.com/MicrochipTech/ml-PIC33CX-BZ2-and-WBZ451-IMU-Datalogger?tab=readme-ov-file)
repo), transmit the acquired data to the ML Model Builder, construct a personalized model capable of accurately classifying the input data, and ultimately deploy the model onto the Machine Learning Evaluation Kit.   

## Hardware Used
 * | ![curiosity development board ](Images/wsg.png) |
   | :--: |
   | *[PIC32CX-BZ2 and WBZ451 Curiosity Development Board](https://www.microchip.com/en-us/development-tool/ev96b94a)* |

 * | ![IMU ](Images/2.PNG) |
   | :--: |
   | *[Mikroe 6DOF IMU 17 CLICK](https://www.microchip.com/en-us/development-tool/ev96b94a)* |

 * | ![Mounting Putty ](Images/Picture1.png) |
   | :--: |
   | *Mounting Putty* |

 * | ![Dumbbell](Images/Picture2.png) |
   | :--: |
   | *Dumbbell* |  
  

## Software Used
* [MPLAB® X IDE](https://microchip.com/mplab/mplab-x-ide)
* [MPLAB® XC32 Compiler](https://microchip.com/mplab/compilers)
* [MPLAB® Code Configurator](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator)
* [MPLAB® ML Model Builder](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=MPLAB-ML-Documentation&redirect=true)

## Setup
The kit is mounted on one end of the dumbbell using double-sided sticky tape and rubber bands. The USB cable is tied around the handle to prevent the kit from being pulled off.
  | ![Dumbbell](Images/Picture3.jpg) |
  | :--: |
  | *Smartbell setup* |  



## Data Collection
The [dataset](/Dataset) used for the development of this application consists of data files of Overhead press, bicep curl and lateral raises exercises.

             ![Exercises](Images/macros.png)
## Data Collection Firmware
A binary build of the data logging firmware used in the data collection for this project can be found in the [binaries folder of this repo](/binaries); to build data logging firmware for different  configurations, visit the [ml-PIC33CX-BZ2 and WBZ451 data logger]([https://github.com/MicrochipTech/ml-dsPIC33CK-LVMC-Data-Logger](https://github.com/MicrochipTech/ml-PIC33CX-BZ2-and-WBZ451-IMU-Datalogger)) repository.

## Firmware Operation

The behavior of the firmware can be summarized as operating in one of the distinct states described in the table below.

| UART Terminal output           | Description                                                        |
| ------------------------------ |  ------------------------------------------------------------ |
| Idle                           | The dumbbell is stationary                                   |
| Bicep curl                     | The user is performing the Bicep curl exercise using the smartbell         |
| Shoulder Press                 | The user is performing a Shoulder press exercise using the smartbell                         |
| Lateral Raise                  | The user is performing a lateral raise exercise using the smartbell   |
| ERROR                          | The Model is not able to classify the project correctly |

## Running the application Demo

- Start MPLAB X IDE and open the project `wbz451-smartbell-lib/ml-wbz451-imu-smartbell-lib.X` with device selection dsPIC33CK256MP508.

- Set the project `ml-wbz451-imu-smartbell-lib.X` as the main project.

- Open the `app_config.h` file located under Header Files. 

- Ensure that the macro **`DATA_STREAMER_FORMAT`** is set as **`DATA_STREAMER_FORMAT_NONE`**

​                               
                       ![macros](Images/macros.png)

 

- Open project properties and ensure that the selected MPLAB® XC32 Compiler and Device Pack support the device configured in the firmware. 

- Build the project and program the device. 

- Open the terminal window on Data Visualizer, select the COM port of the LVMC board, and set the below configurations
  - Baud rate 115200
  - Data bits 8
  - Stop bits 1
  - Parity None



## Classifier Performance
Below is the confusion matrix for the test dataset. Note that the classes are highly imbalanced so accuracy is not a good indicator of overall performance.

![Test set confusion matrix](assets/confusion_matrix.png)

