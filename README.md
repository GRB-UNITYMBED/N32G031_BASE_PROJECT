# UnityMbed Base Project (N32G031)

[English](#english) | [ภาษาไทย](#ภาษาไทย)

---

<h2 id="english">🇺🇸 English</h2>

Welcome to the Base Project for the N32G031 microcontroller development board, perfectly optimized for seamless integration with the AI Assistant in the IDE.

You can use this project as a foundation to create intelligent systems, ranging from basic projects to HMI panels or industrial-grade control systems.

## Folder Structure

The main structure is as follows:

*   **`src/`** : Folder for main code files (`main.c`) and External Library `.c` files (e.g., `oled.c`, `dht11.c`).
*   **`inc/`** : Folder for Header `.h` files (e.g., `oled.h`, `fonts.h`).
*   **`drivers/`** : Folder containing the Standard Peripheral Library for the N32G031 chip (Do not delete or modify).

## 🛠️ Getting Started

1. **Code with AI:**
   Open the `src/main.c` file. You will find a prepared blank structure. Type your requirements into the AI Assistant to start creating instantly.
2. **Adding External Libraries:**
   If you want to use an OLED display, Servo Motor, or various sensors:
   - Drop the `.c` files into the `src/` folder.
   - Drop the `.h` files into the `inc/` folder.
   - *The system will automatically discover and build the files for you!*
3. **Build & Flash:**
   Click the **Build** and **Flash** buttons on the IDE to upload the program to your N32G031 board.

## AI Prompting Tips (The CLEAR Framework)

To get accurate and functional code on our platform, we recommend prompting the AI Assistant using the **CLEAR** structure:

*   **C - Context:** Always specify that you are using the "N32G031" board and the "Standard Peripheral Library".
*   **L - Logic/Length:** Clearly explain the logic and working conditions step-by-step.
*   **E - Exact Information:** Specify the exact hardware pins used (e.g., LED connected to pin PA2).
*   **A - Action:** State clearly whether you want the AI to write code, fix a bug, or explain a concept.
*   **R - Refine:** If an error occurs, paste the Error Log directly into the AI for analysis and correction.

---

<h2 id="ภาษาไทย">🇹🇭 ภาษาไทย</h2>

ยินดีต้อนรับสู่โปรเจคตั้งต้น (Base Project) สำหรับบอร์ดพัฒนาไมโครคอนโทรลเลอร์ N32G031 ที่พร้อมทำงานร่วมกับ AI Assistant บน IDE ได้อย่างสมบูรณ์แบบ 

คุณสามารถใช้โปรเจคนี้เป็นรากฐานในการสร้างสรรค์ระบบอัจฉริยะ ตั้งแต่โปรเจคพื้นฐานไปจนถึง HMI Panel หรือระบบควบคุมระดับอุตสาหกรรม

## โครงสร้างโฟลเดอร์ (Folder Structure)

โครงสร้างหลักมีดังนี้:

*   **`src/`** : โฟลเดอร์สำหรับใส่ไฟล์โค้ดหลัก (`main.c`) และไฟล์ External Library นามสกุล `.c` (เช่น `oled.c`, `dht11.c`) 
*   **`inc/`** : โฟลเดอร์สำหรับใส่ไฟล์ Header นามสกุล `.h` (เช่น `oled.h`, `fonts.h`)
*   **`drivers/`** : โฟลเดอร์เก็บ Standard Peripheral Library ของชิป N32G031 (ห้ามลบหรือแก้ไข)

## 🛠️ วิธีการใช้งาน (Getting Started)

1. **เขียนโค้ดร่วมกับ AI:** 
   เปิดไฟล์ `src/main.c` ขึ้นมา คุณจะพบโครงสร้างที่เตรียมไว้ให้ พิมพ์ความต้องการของคุณลงใน AI Assistant เพื่อเริ่มต้นสร้างสรรค์ผลงานได้ทันที
2. **การเพิ่มอุปกรณ์เสริม (External Libraries):**
   หากคุณต้องการใช้งานจอ OLED, Servo Motor หรือเซนเซอร์ต่าง ๆ:
   - นำไฟล์ `.c` ไปวางในโฟลเดอร์ `src/`
   - นำไฟล์ `.h` ไปวางในโฟลเดอร์ `inc/`
   - *ระบบจะทำการค้นหาและ Build ไฟล์ให้คุณโดยอัตโนมัติ!*
3. **Build & Flash:**
   กดปุ่ม Build และ Flash บน IDE เพื่ออัปโหลดโปรแกรมลงสู่บอร์ด N32G031

## AI Prompting Tips (สูตรการสั่งงาน AI)

เพื่อให้ได้โค้ดที่แม่นยำและใช้งานได้จริงบนแพลตฟอร์มของเรา แนะนำให้สั่งงาน AI Assistant ด้วยโครงสร้างแบบ **CLEAR**:

*   **C - Context (บริบท):** แจ้งเสมอว่าใช้บอร์ด "N32G031" และใช้ "Standard Peripheral Library"
*   **L - Logic/Length (ลอจิก/ความยาว):** อธิบายเงื่อนไขการทำงานให้ชัดเจนเป็นลำดับขั้นตอน
*   **E - Exact Information (ข้อมูลเฉพาะ):** ระบุขาพอร์ตที่ต่อใช้งานให้ชัดเจน (เช่น ต่อ LED ที่ขา PA2)
*   **A - Action (สิ่งที่ต้องทำ):** ระบุว่าต้องการให้ AI เขียนโค้ด, แก้บั๊ก, หรืออธิบายการทำงาน
*   **R - Refine (ปรับแต่ง):** หากเกิด Error ให้นำ Error Log มาโยนให้ AI วิเคราะห์เพื่อแก้ไข
