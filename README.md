# Chip8-Emulator
A simple emulator for chip 8 programs

## Authors

- [@MathNosseb](https://github.com/MathNosseb)

## Intructions references

| Intructions             | Hex                                                               | 
| - | - | 
| CLS | 00E0 | 
| RET | 00EE | 
| SYS addr | 0nnn | 
| JP addr | 1nnn | 
| CALL addr | 2nnn | 
| SE Vx, byte | 3xkk | 
| SNE Vx, byte | 4xkk | 
| SE Vx, Vy | 5xy0 | 
| LD Vx, byte | 6xkk | 
| ADD Vx, byte | 7xkk | 
| LD Vx, Vy | 8xy0 | 
| OR Vx, Vy | 8xy1 | 
| AND Vx, Vy | 8xy2 | 
| XOR Vx, Vy | 8xy3 | 
| ADD Vx, Vy | 8xy4 | 
| SUB Vx, Vy | 8xy5 | 
| SHR Vx {, Vy} | 8xy6 | 
| SUBN Vx, Vy | 8xy7 | 
| SHL Vx {, Vy} | 8xyE |
| SNE Vx, Vy | 9xy0 |
| LD I, addr | Annn |
| JP V0, addr | Bnnn |
| RND Vx, byte | Cxkk |
| DRW Vx, Vy, nibble | Dxyn |
| SKP Vx | Ex9E |
| SKNP Vx | ExA1 |
| LD Vx, DT | Fx07 |
| LD Vx, K | Fx0A |
| LD DT, Vx | Fx15 |
| LD ST, Vx | Fx18 |
| LD F, Vx | Fx29 |
| LD B, Vx | Fx33 |
| LD [I], Vx | Fx55 |
| LD Vx, [I] | Fx65 |

 
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
