<img width="1280" height="720" alt="title" src="https://github.com/user-attachments/assets/9f2a17f2-83f8-4b22-bb7b-61393436f2b0" />

<div align="center">
  <h1>beatplate</h1>
  <p>A Rhythm Game project made for learning C++</p>
</div>

---

### About:

Beatplate is a rhythm game created by me (**lAg**) with the purpose of learning C++ and diving deeper into low-level code.

i’m aiming to finish the project with:
- The core mechanics done
- Decent visuals
- A Game Editor

The game is heavily inspired by the rhythm game **osu!**.

---

### Technologies:

- **Language:** C++23  
- **Libraries used:**
  - SFML 3.0.2 → https://github.com/SFML/SFML  
  - nlohmann json → https://github.com/nlohmann/json

---

### Status:

Currently in development.  
Focusing on core gameplay logic first.

---

### How to Build the Project:

You can build the game with simple cmake terminal commands:

### Windows:

For Windows You'll Need to Download the SFML Library and put it in the /libs folder.

PowerShell: (MinGW)

```
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
New-Item -ItemType Directory build | Out-Null
Set-Location build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
mingw32-make -j %NUMBER_OF_PROCESSORS%
```

CMD: (MinGW)

```
rmdir /s /q build
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
mingw32-make -j %NUMBER_OF_PROCESSORS%
```

### Linux: 

For Linux You'll Need to Install SFML from your package manager.

Example: (Arch)

```
sudo pacman -S sfml
```

Shell Script:

```
rm -rf build && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(nproc)
```


