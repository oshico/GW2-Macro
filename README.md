# 🔧 GW2-Macro Manager Addon

A C++ toolkit for managing **Guild Wars 2 macros** via the **Nexus Loader**. This project provides cross-platform build scripts, integrates with the **Nexus API**, and uses **ImGui** for its GUI. It includes tools for compiling, running, and managing macros efficiently on Windows, built from Linux.

---

## 📦 Features

- **Manage up to 10 macros** simultaneously.
- **Customizable macro keybinds** through Nexus Loader.
- **Macro actions** include:
  - Key presses
  - Key releases
  - Delays
- **Allowed macro keys**:
  - Weapon Actions
  - Utility Skill Actions
  - Weapon Swap
  - Dodge
  - Jump

---

## 📁 Project Structure
```
src/
├── src/
│ ├── shared.cpp
│ └── main.cpp
├── include/
│ ├── Nexus.h
│ └── shared.h
CMakeLists.txt
build.sh
```
---

## 🚀 Getting Started

### ✅ Prerequisites

- **C++17 or higher**
- **CMake 3.15+**
- **Linux (for cross-compiling to Windows)**
- **Nexus API headers and libraries**
- **ImGui library**

---

### 📥 Installation

#### 1. Clone the repository

```
git clone https://github.com/oshico/GW2-Macro.git
cd GW2-Macro/
```
#### 2. Build the project

```
chmod +x build.sh
./build.sh
```
---
### ⚙️ Usage

Copy the generated .dll file to your Guild Wars 2 addons folder.</br>
Launch GW2 and enable the macro manager via Nexus Loader.

### ✅ License

GNU General Public License — see LICENSE for details.

### ✍️ Author

oshico

### 📬 Feedback or Issues?

Open an issue or submit a pull request on GitHub.

Contact via GitHub profile: oshico
