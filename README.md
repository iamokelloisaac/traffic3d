```markdown
# 🚦 Traffic3D: A Terminal-Based 3D Traffic Simulation

![Language](https://img.shields.io/badge/Language-C23-blue.svg)
![Build](https://img.shields.io/badge/Build-CMake-green.svg)
![Graphics](https://img.shields.io/badge/Graphics-Notcurses-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20WSL-lightgrey.svg)

**Traffic3D** is a fully custom, 3D software-rendered traffic intersection simulation that runs entirely inside your terminal. Built from scratch in C, it bypasses standard text characters to draw real, pixel-perfect 3D graphics using the Kitty Graphics Protocol. 

Featuring AI-driven vehicles, procedurally generated pedestrians (penguins!), and a complete traffic state machine, it transforms your terminal into a living, breathing diorama.

---

## ✨ Features

* **True 3D Software Rendering:** Custom math, Z-buffering, and rasterization. No OpenGL, no GPU required.
* **Procedural Generation:** Every penguin is unique, with varying sizes, colors, and waddle speeds.
* **Smart AI Drivers:** Cars accelerate, brake, queue behind each other, and will *reverse* out of the crosswalk if they get caught on it during a red light.
* **Dynamic Traffic Logic:** A fully configurable state machine managing green, yellow, and red transitions with flashing warnings.
* **2D HUD & Vector Icons:** A heads-up display with a giant traffic light featuring smooth, mathematically drawn car and penguin icons that sync with the simulation.
* **Terminal Audio:** Rhythmic terminal bell alerts during pedestrian crossings.

---

## 🛠️ Technical Setup (Installation)

### Prerequisites & Terminal Requirements
For the best experience (True Pixel Graphics), you **must** use a terminal emulator that supports the Kitty Graphics Protocol. 
* **Recommended:** `kitty`, `wezterm`, or `foot`.
* *Note: If you use standard terminals (like xterm or the default CLion terminal), the program will automatically fall back to lower-resolution text blocks.*

### Option 1: Native Linux (Debian/Ubuntu/Kali)
Open your terminal and install the required C compiler, git, and graphics libraries:

```bash
sudo apt update
sudo apt install build-essential git pkg-config libnotcurses-dev cmake
```

*(Optional but highly recommended: Install the `kitty` terminal for true pixel rendering)*
```bash
sudo apt install kitty
```

### Option 2: Windows (via WSL2)
Windows users can run this perfectly using the Windows Subsystem for Linux (WSL2).

1. Open **PowerShell** as Administrator and run:
   ```powershell
   wsl --install -d Ubuntu
   ```
2. Restart your computer and set up your Ubuntu username/password.
3. Open **Windows Terminal**, select Ubuntu, and run the Linux installation commands:
   ```bash
   sudo apt update
   sudo apt install build-essential git pkg-config libnotcurses-dev cmake
   ```
4. **For True Pixel Graphics**, you need a graphics-capable terminal inside WSL:
   ```bash
   sudo apt install kitty
   kitty
   ```

---

## 🚀 Build & Run

Once your dependencies are installed, clone the repository and build the project using CMake:

```bash
# 1. Clone the repository into a folder named traffic3d
git clone https://github.com/iamokelloisaac/traffic3d.git

# 2. Navigate into the project folder
cd traffic3d

# 3. Configure the build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

# 4. Compile the source code
cmake --build . -j

# 5. Run the simulation
kitty ./traffic3d
```

After running the app for first time on wsl windows, enlarge its screen to full and quit. Then relaunch so that to correct the glitches

---

## 📖 User Manual: How It Works

The simulation mimics a real-world pedestrian crossing at a busy intersection. Here is how the world behaves:

### The Traffic Lights
The system uses a strict safety sequence. When a pedestrian wants to cross, they press a button.
1. The **Vehicle Light** flashes yellow, then turns solid red (cars brake and stop).
2. The **Pedestrian Light** flashes green, then turns solid green (penguins start crossing).
3. After a set time, the pedestrian light flashes green as a warning (penguins hurry), then turns red.
4. Finally, the vehicle light turns green and traffic resumes.

### The Invisible Stop Line
Cars are programmed to stop *before* the zebra crossing, leaving a wide gap so the penguins don't bump into the stopped vehicles.

### The "Smart Driver" Reverse Gear
Sometimes, a light turns red while a car is already halfway across the crosswalk. Instead of blocking the penguins, that car will automatically **reverse** (back up) out of the crosswalk. Any cars stuck behind it will also reverse to give it room, keeping the path completely clear.

### The Penguin Queue
If you call the crossing while the lights are already changing, the system registers a "Queued Call." The current crossing finishes, the system waits for a cool-off period (default: 10 seconds), and then automatically runs the queued crossing. You will see a "CALL QUEUED..." message on the screen when this happens.

---

## 🎮 Controls & Configuration

### Keyboard Controls

| Key | Action |
| :--- | :--- |
| **`P`** | Call the pedestrian button (starts the crossing sequence). |
| **`Q`** / **`Esc`** | Safely quit the simulation. |

### The Configuration File
The entire simulation is driven by a configuration file located at `config/default.conf`. You can open this file in any text editor (like Notepad, VS Code, or Nano) to tweak the timings, colors, and behaviors without touching the code.

For example, if you want the penguins to cross faster, open the file and change:
```ini
pedestrian_walk_seconds = 15.0
```
To:
```ini
pedestrian_walk_seconds = 5.0
```
Save the file, restart the simulation, and your new settings will take effect immediately.

---
*Built with 💻, C, and a lot of ☕.*
```