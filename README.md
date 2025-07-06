# LocalAI
An application made in c++ that allows you to use AI locally with a ChatGPT style interface

## Prerequisites

### Install Ollama
Before using LocalAI, you need to install Ollama to run local AI models:

1. **Download and install Ollama** from [https://ollama.ai](https://ollama.ai)
2. **Start Ollama** (it typically runs as a service after installation)
3. **Pull a model** to get started:
   ```bash
   ollama pull llama2
   ```
   or for a smaller model:
   ```bash
   ollama pull llama2:7b
   ```

Once Ollama is installed and running, you can use LocalAI to interact with your local models through a familiar ChatGPT-style interface.
