# 🔧 GW2-Macro Manager Addon

A C++ toolkit for managing **Guild Wars 2 macros** via the **Nexus Loader**.  
It integrates with the **Nexus API**, uses **ImGui** for its GUI, and supports cross-platform builds (Linux → Windows).  

👉 A prebuilt **.dll release** is already available for players who just want to use the addon.  
👉 Developers or testers can also **compile it themselves** by following the quick guide below.  

---

## 📦 Features

- **Manage up to 10 macros** simultaneously  
- **Customizable macro keybinds** through Nexus Loader  
- **Macro actions** include:
  - Key presses
  - Key releases
  - Delays  
- **Allowed macro keys**:
  - Weapon Actions
  - Utility Skills
  - Weapon Swap
  - Dodge
  - Jump  

---

# ⚡ How to Use (Prebuilt DLL)

1. Download the latest `.dll` from the [Releases](./releases) page.  
2. Copy it into your **Guild Wars 2 addons folder**.  
3. Launch GW2 and enable the macro manager via **Nexus Loader**.  

---

# 🛠️ Compiling It Yourself (Optional)

## ✅ Prerequisites

- **Linux** (for cross-compiling to Windows)  
- **C++17 or higher**  
- **CMake 3.15+**  
- **Nexus API headers + libraries**  
- **ImGui library**  

---

## 🚀 Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/oshico/GW2-Macro.git
cd GW2-Macro/

# 2. Make the build script executable
chmod +x build.sh

# 3. Build the project
./build.sh
```

The compiled .dll will appear in the build/ directory.

### ✅ License

GNU General Public License — see LICENSE for details.

### ✍️ Author

oshico

### 📬 Feedback or Issues?

Open an issue or submit a pull request on GitHub.

Contact via GitHub profile: oshico
