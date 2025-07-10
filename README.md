# LocalAI

LocalAI is a C++/Qt 6 application that provides a ChatGPT-style interface to interact with local AI models using [Ollama](https://ollama.ai). It leverages QML for the UI and supports modern local LLMs with minimal overhead.

---

## 🚀 Features

- Local AI chat interface
- Modern QML-based GUI
- Integration with Ollama for local model inference

---

## 🧰 Prerequisites

### ✅ Qt 6.5 or newer

Install Qt with the following modules:

- `Qt Quick`
- `Qt Sql`

You can install via [Qt Online Installer](https://www.qt.io/download) or through your package manager.

### ✅ CMake 3.16 or newer

Install CMake:

- On Linux: `sudo apt install cmake`
- On macOS: `brew install cmake`
- On Windows: via [cmake.org](https://cmake.org/download/)

### ✅ Compiler

You’ll need a C++17-compatible compiler (GCC, Clang, or MSVC).

---

## 🧠 Install Ollama

LocalAI requires [Ollama](https://ollama.ai) to run local language models.

1. Download and install Ollama from [https://ollama.ai](https://ollama.ai)
2. Start Ollama (it typically runs as a background service)
3. Pull a model:
   ```bash
   ollama pull llama2
   ```
   or a smaller version:
   ```bash
   ollama pull llama2:7b
   ```

---

## 🔧 Building the Project

### Clone the repository

```bash
git clone https://github.com/your-username/LocalAI.git
cd LocalAI
```

### Configure and build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

> 💡 If Qt isn’t found automatically, you may need to set `CMAKE_PREFIX_PATH`:
> ```bash
> export CMAKE_PREFIX_PATH=/path/to/Qt/6.5.0/gcc_64/lib/cmake
> ```

---

## ▶️ Running the App

From the `build/` directory:

```bash
./appLocalAI
```

(On Windows: `appLocalAI.exe`)

---

## 📁 Project Structure

```
LocalAI/
├── CMakeLists.txt
├── main.cpp
├── Main.qml
├── Label.qml
├── HistoryItem.qml
├── AiHandler.h / .cpp
└── ollama.hpp
```

---

## 📦 Installation (Optional)

To install the application system-wide (e.g., on Linux):

```bash
cmake --install .
```

---

## 📤 Deployment Notes

You may need to bundle Qt runtime libraries when distributing:

- **Linux**: Use `linuxdeployqt`
- **Windows**: Use `windeployqt`
- **macOS**: Use `macdeployqt`

---
