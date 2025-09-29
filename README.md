# CRM3007L

OB38R16T1W28SP

|PORT  |CONN       |DESC                                  |SW         | I/O |  AF  |
|------|-----------|--------------------------------------|-----------|-----|------|
| P0.0 | SW        |                                      | STEAM     |  I  |      |
| P0.1 | LED       | LED                                  |           |  O  |      |
| P0.2 | PCB-C.6   |                                      |           |  O  |      |
| P0.3 | PCB-C.7   |                                      |           |  O  |      |
| P0.4 | NTC1      | COFFEE BOILER, 10K NTC, 1K pullup 5V |           |  I  | ADC0 |
| P0.5 | NTC2      | STEAM BOILER, 10K NTC, 1K pullup 5V  |           |  I  | ADC1 |
| P0.6 | PCB-C.2   | 10K pulldown to GND                  |           |  I  | ADC2 |
| P0.7 | PCB-C.3   |                                      |           |  O  |      |
| P1.0 | PCB-C.4   |                                      |           |     |      |
| P1.1 | PUMP      |                                      |           |  O  | PWM1 |
| P1.3 | Z-C       |                                      |           |  I  | INT0 |
| P1.6 | H2        | STEAM BOILER                         |           |  O  | PWM2 |
| P1.7 | H1        | COFFEE BOILER                        |           |  O  | PWM3 |
| P2.3 | PCB-C.5   |                                      |           |  O  |      |
| P2.5 | PCB-D.TX  |                                      |           |     | TX_1 |
| P2.6 | PCB-D.RX  |                                      |           |     | RX_1 |
| P3.0 | K3        | COFFEE VALVE                         |           |  O  |      |
| P3.1 | K2        | STEAM VALVE                          |           |  O  |      |
| P3.2 | K1        | 3WV                                  |           |  O  |      |
| P3.3 |           | BUZ                                  |           |  O  |      |

| |PCB-C |                              |
|-|------|------------------------------|
|1|5V    |                              |
|2| P0.6 | PWR=1K, S1=10K, S2=30K to 5V |
|3| P0.7 | LED common                   |
|4| P1.0 |                              |
|5| P2.3 | PWR LED                      |
|6| P0.2 | S1 LED                       |
|7| P0.3 | S2 LED                       |

|   |PCB-D |
|---|------|
|5V |      |
|RX | P2.6 |
|TX | P2.5 |
|GND|      |
