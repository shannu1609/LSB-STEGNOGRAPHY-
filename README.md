LSB Steganography Project – Hiding Data in Images

This project demonstrates image steganography, a technique used to hide secret data inside digital images without noticeably changing the image itself. The main goal is to provide a simple yet effective way to conceal information securely using pixel manipulation.

🚀 Features

Hide text messages inside images using LSB (Least Significant Bit) technique

Extract hidden data from stego-images

Support for .bmp formats (lossless image types)

Simple command-line interface (CLI) or GUI version (if you built one)

Error detection and message validation

🧠 Concept

Steganography works by altering the least significant bits of image pixels to encode message data. Since these bit changes are minimal, the human eye cannot detect the difference between the original and the stego image.

🛠️ Technologies Used

Language: C

Platform: Windows / Linux

📂 Project Structure
📁 Steganography-Project
 ┣ 📄 main.c 
 ┣ 📄 encode.c 
 ┣ 📄 decode.c
 ┣ 📄 README.md
 ┗ 📂 images
     ┣ beautiful_image.bmp
     ┗ default_image.bmp
