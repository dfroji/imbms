# ImBMS

ImBMS is a BMS chart editor for Linux built with [ImGui-SFML](https://github.com/SFML/imgui-sfml) using [SoLoud](https://github.com/jarikomppa/soloud) for audio. It's still very much a work in progress with a lot of functionality and QoL features missing, but it is possible to create a proper chart using it. 

![Main view of ImBMS](https://github.com/user-attachments/assets/e353a610-9f64-4aef-b6b3-4092f8616c88)

## Build with CMake
### Requirements
- SDL2 ([sdl2](https://archlinux.org/packages/?name=sdl2))
- ICU ([icu](https://archlinux.org/packages/?sort=&q=icu))
- Fontconfig ([fontconfig](https://archlinux.org/packages/?sort=&q=fontconfig))
- VL Gothic Regular ([ttf-vlgothic](https://archlinux.org/packages/?sort=&q=ttf-vlgothic))
### Building
```
$ mkdir build && cd build
$ cmake ..
$ make
```

## TODO

- Copy and paste notes
- Selecting and manipulating multiple notes
- Select region
- Support for long notes and hidden notes (right now they just aren't rendered and they can't be interacted with)
- Playback
- Keyboard input to move BGM notes to play channels
- Flesh out the file dialog
- Plenty more I'm probably forgetting
