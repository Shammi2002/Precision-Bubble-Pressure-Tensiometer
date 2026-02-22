# Precision Surface Tension Instrument 🧪
## Maximum Bubble Pressure Method | EE322 Embedded System Design

[![Microcontroller](https://img.shields.io/badge/MCU-ATmega328P-blue?style=flat-square&logo=microchip)](https://www.microchip.com/en-us/product/atmega328p)
[![Language](https://img.shields.io/badge/Language-Assembly-red?style=flat-square)](https://en.wikipedia.org/wiki/Assembly_language)


### 📋 Project Overview
This project presents the design and implementation of a **Precision Surface Tension Instrument** developed as part of the EE322 Embedded Systems module at [Insert University Name]. 

Surface tension measurement is critical in pharmaceuticals, food stability, and chemical engineering. Our system automates the **Maximum Bubble Pressure Method**, providing a high-precision, cost-effective alternative to manual laboratory techniques by leveraging real-time sensor fusion and low-level hardware optimization.

---

### ⚙️ System Features
* **Automated Data Acquisition:** Real-time monitoring of pressure and immersion depth.
* **Low-Level Optimization:** Core processing logic implemented in **Assembly (AVR)** for maximum timing accuracy and minimal latency.
* **High Precision:** Automated detection of the maximum pressure point during bubble formation.
* **Cost-Effective Design:** Built using an ATmega328P architecture, suitable for research and quality control environments.

---

### 🔬 The Methodology: Maximum Bubble Pressure
The system calculates surface tension ($\gamma$) by measuring the pressure required to form a bubble at the end of a capillary tube immersed in a liquid. The instrument solves for:
      $$\gamma = \frac{(P_{max} - P_{hydrostatic}) \cdot r}{2}$$
*Where $P_{max}$ is the peak pressure measured, $r$ is the capillary radius, and $P_{hydrostatic}$ is the pressure due to depth.*

---

### 💻 Software Architecture
The software is engineered for **determinism and efficiency**. By using Assembly language for critical paths, we ensure that the ADC (Analog-to-Digital Converter) sampling matches the bubble frequency perfectly.

1.  **Initialization:** Setting up I/O ports, ADC, and Timers.
2.  **Sampling Loop:** High-speed capture of pressure sensor data.
3.  **Peak Detection Algorithm:** Identifying the exact moment of bubble detachment.
4.  **Display Logic:** Converting raw sensor values into physical units (mN/m).

---

### 📂 Repository Contents
* `/src`: AVR Assembly source code for the ATmega328P.
* `/schematics`: Circuit diagrams and PCB layout files.
* `/docs`: [Project Proposal (PDF)](https://github.com/user-attachments/files/24689494/Project._proposal_Group_11.pdf).
* `/simulations`: Proteus or MultiSim simulation files.

---

### 👥 Group 11 Members
* [E/20/138]
* [E/20/329]
* [E/20/392]

---
*Developed for the EE322 Embedded System Design course.*
