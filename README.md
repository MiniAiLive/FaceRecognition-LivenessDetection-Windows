<div align="center">
   <h1> MiniAiLive Face Recognition Windows(C++) SDK </h1>
   <img src=https://miniai.live/wp-content/uploads/2024/02/logo_name-1-768x426-1.png alt="MiniAiLive Logo"
   width="300">
</div>

## Welcome to the [MiniAiLive](https://www.miniai.live/)!
This repository contains a C++ application that demonstrates face recognition capabilities using computer vision techniques. The demo utilizes OpenCV and dlib libraries for efficient face detection and recognition with 3D passive face liveness detection (face anti-spoofing).

![demo](https://github.com/MiniAiLive/MiniAIFaceSDK/assets/136824647/0859737c-d79e-440f-a400-5aa9097ed818)
### Key Features
 - ### [Face detection](https://demo.miniai.live/): 
    ![face_detect](https://github.com/MiniAiLive/MiniAIFaceSDK/assets/136824647/61f02463-09e9-4efa-b904-6f2f453d95d5)

    The SDK utilizes advanced computer vision techniques to detect faces in images or video frames, enabling a wide range of applications. 
 - ### [Face recognition](https://demo.miniai.live/):
 ![face_compare](https://github.com/MiniAiLive/MiniAIFaceSDK/assets/136824647/29fcd277-f850-4b83-a5da-dc63fb5f0af7)

    It can recognize known faces by comparing them with a pre-defined database of individuals.
 - ### [Age estimation](https://demo.miniai.live/): 
    It can estimate the age of detected faces.
 - ### [Gender detection](https://demo.miniai.live/):
    It can determine the gender of detected faces.
 - ### [Liveness detection](https://demo.miniai.live/): 
    It can detect whether a face is from a live person or a static image.

### Prerequisites
> - C++ compiler with C++11 support  
> - OpenCV (version 4.6.0)

### Installation
Clone the repository:

```
git clone https://github.com/MiniAI/MiniAIFaceSDK.git
```

### Download

- [Download `vcredist_x64.exe` from Microsoft](https://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x64.exe)
- [Download `vc_redist.x64.exe` from Microsoft](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x64.exe)
```java
1. Download the `vcredist_x64.exe`, and `vc_redist.x64.exe` files from the provided link.
2. Double-click the downloaded file to start the installation.
3. Follow the on-screen instructions to complete the installation process.
4. Restart your system if prompted.

For more detailed installation instructions, please refer to the [official Microsoft documentation](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redistributable).

```

### Usage
Run the compiled executable:
```java 
./MiniAIFaceDemo.exe
```
We provide free license to test our SDK according to HWID.
You can get the HWID in our SDK application.
Follow the on-screen instructions to perform face detection and recognition.

![HWID](https://github.com/MiniAiLive/MiniAIFaceSDK/assets/136824647/75d935ff-bf2d-4dd2-ac8f-5b356785f91a)
### Customization
> - Face Database: Replace the sample face database with your own set of known faces. Ensure that you provide clear and properly labeled images for accurate recognition.
> - Recognition Algorithm: Adjust the recognition algorithm parameters or explore other algorithms provided by dlib to optimize the recognition performance.

### Contributing
Contributions are welcome! If you'd like to contribute to this project, please follow these steps:
```java 
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them with descriptive messages.
4. Push your changes to your forked repository.
5. Submit a pull request to the original repository.
```

### Try Online Demo
Please visit our Face API Web Demo here. https://demo.miniai.live

### Related Product
No | Project | Feature
---|---|---|
1 | [MiniAI-Face-Recognition-AndroidSDK](https://github.com/MiniAiLive/MiniAI-Face-Recognition-AndroidSDK) | Face Matching, 3D Face Passive Liveness
2 | [MiniAI-Face-Recognition-iOS-SDK](https://github.com/MiniAiLive/MiniAI-Face-Recognition-iOS-SDK) | Face Matching, 3D Face Passive Liveness
3 | [MiniAI-Face-Recognition-ServerSDK](https://github.com/MiniAiLive/MiniAI-Face-Recognition-ServerSDK) | Face Matching, 3D Face Passive Liveness
4 | [MiniAI-Face-Recognition-WindowsSDK](https://github.com/MiniAiLive/MiniAI-Face-Recognition-WindowsSDK) | Face Matching, 3D Face Passive Liveness
5 | [MiniAI-Face-LivenessDetection-AndroidSDK](https://github.com/MiniAiLive/MiniAI-Face-LivenessDetection-AndroidSDK) | 3D Face Passive Liveness
6 | [MiniAI-Face-LivenessDetection-iOS-SDK](https://github.com/MiniAiLive/MiniAI-Face-LivenessDetection-iOS-SDK) | 3D Face Passive Liveness
7 | [MiniAI-Face-LivenessDetection-ServerSDK](https://github.com/MiniAiLive/MiniAI-Face-LivenessDetection-ServerSDK) | 3D Face Passive Liveness
8 | [MiniAI-Face-Matching-AndroidSDK](https://github.com/MiniAiLive/MiniAI-Face-Matching-AndroidSDK) | 1:1 Face Matching
9 | [MiniAI-Face-Matching-iOS-SDK](https://github.com/MiniAiLive/MiniAI-Face-Matching-iOS-SDK) | 1:1 Face Matching
10 | [MiniAI-Face-Attributes-AndroidSDK](https://github.com/MiniAiLive/MiniAI-Face-Attributes-AndroidSDK) | Face Attributes

## About MiniAiLive
[MiniAiLive](https://www.miniai.live/) is a leading AI solutions company specializing in computer vision and machine learning technologies. We provide cutting-edge solutions for various industries, leveraging the power of AI to drive innovation and efficiency.

## Contact US
For any inquiries or questions, please [Contact US](https://www.miniai.live/contact/)

<p align="center">
<a target="_blank" href="https://t.me/Contact_MiniAiLive"><img src="https://img.shields.io/badge/telegram-@MiniAiLive-blue.svg?logo=telegram" alt="www.miniai.live"></a>&emsp;
<a target="_blank" href="https://wa.me/+19162702374"><img src="https://img.shields.io/badge/whatsapp-MiniAiLive-blue.svg?logo=whatsapp" alt="www.miniai.live"></a>&emsp;
<a target="_blank" href="https://join.skype.com/invite/ltQEVDmVddTe"><img src="https://img.shields.io/badge/skype-MiniAiLive-blue.svg?logo=skype" alt="www.miniai.live"></a>&emsp;
</p>
