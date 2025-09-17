# Chip8-Emulator
A simple emulator for chip 8 programs

## Authors

- [@MathNosseb](https://github.com/MathNosseb)

## Intructions references

| Intructions             | Hex                                                               | 
| - | - | 
| CLS | 00E0 | | SHL Vx {, Vy} | 8xyE |
| RET | 00EE | | SNE Vx, Vy | 9xy0 |
| SYS addr | 0nnn | | LD I, addr | Annn |
| JP addr | 1nnn | | JP V0, addr | Bnnn |
| CALL addr | 2nnn | | RND Vx, byte | Cxkk |
| SE Vx, byte | 3xkk | | DRW Vx, Vy, nibble | Dxyn |
| SNE Vx, byte | 4xkk | | SKP Vx | Ex9E |
| SE Vx, Vy | 5xy0 | | SKNP Vx | ExA1 |
| LD Vx, byte | 6xkk | | LD Vx, DT | Fx07 |
| ADD Vx, byte | 7xkk | | LD Vx, K | Fx0A |
| LD Vx, Vy | 8xy0 | | LD DT, Vx | Fx15 |
| OR Vx, Vy | 8xy1 | | LD ST, Vx | Fx18 |
| AND Vx, Vy | 8xy2 | | ADD I, Vx | Fx1E |
| XOR Vx, Vy | 8xy3 | | LD F, Vx | Fx29 |
| ADD Vx, Vy | 8xy4 | | LD B, Vx | Fx33 |
| SUB Vx, Vy | 8xy5 | | LD [I], Vx | Fx55 |
| SHR Vx {, Vy} | 8xy6 | | LD Vx, [I] | Fx65 |
| SUBN Vx, Vy | 8xy7 | 


## Deployment on your machine

To use it download the repo

```bash
  git clone https://github.com/MathNosseb/Chip8-Emulator.git
```
compile with SDL and run with the ch8 file in parameter

```bash
  cd Chip8-Emulator/
  gcc main.c -o main -lSDL2
  ./main game.ch8
```
for windows compile in an executable (.exe) and run with your game in parameter
```bash
  main.exe game.ch8
```
## Documentation

[C8TECH10](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

[university of Paris](https://sysblog.informatique.univ-paris-diderot.fr/2024/05/07/premier-pas-dans-lemulation-chip8-en-c/)
