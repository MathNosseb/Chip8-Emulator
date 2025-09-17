#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <SDL2/SDL.h>


struct Chip8 {
    unsigned char memory[4096];
    uint8_t V[16];
    uint16_t I; // Index registeraine instruction
    uint8_t delay;
    uint8_t sound;
    uint16_t PC; // Program counter -> il conte l adresse de la prochaine instruction
    uint8_t SP; //stack pointer
    uint16_t stack[16];
    uint8_t keys[16]; // 1 = down, 0 = up
    uint8_t gfx[64*32];
    size_t size;
    uint_fast8_t drawflag;
    
};

uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


struct Chip8 chip8;
clock_t STARTtimer,ENDtimer;
SDL_Renderer * renderer;
SDL_Window *win;
SDL_Texture * screen;


void load_cartouche_in_memory(const char *filename)
{
    FILE *fptr;
    fptr = fopen(filename, "rb");
    if (!fptr)
    {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }

    // Aller à la fin du fichier
    fseek(fptr, 0, SEEK_END);


    // Récupérer la position actuelle = taille du fichier
    long size = ftell(fptr);

    // Revenir au début du fichier
    rewind(fptr);

    unsigned char * buffer = malloc(size);


    printf("Taille du fichier : %ld octets\n", size);

    // Read the content and store it inside buffer
    size_t bytesRead = fread(buffer, 1, size, fptr);
    chip8.size = bytesRead;

    size_t offset = 0x200;

    for (size_t i = 0; i < bytesRead; i++) {
        chip8.memory[i+offset] = buffer[i];
        
    }

    printf("\n\n");
    fclose(fptr);
    free(buffer);
}

void update_timer(){
    if (chip8.delay > 0) chip8.delay--;
    if (chip8.sound > 0) chip8.sound--;
}

void handle_keyboard(SDL_Event event) {
    int pressed = (event.type == SDL_KEYDOWN);

    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_1: chip8.keys[0x1] = pressed; break;
        case SDL_SCANCODE_2: chip8.keys[0x2] = pressed; break;
        case SDL_SCANCODE_3: chip8.keys[0x3] = pressed; break;
        case SDL_SCANCODE_4: chip8.keys[0xC] = pressed; break;

        case SDL_SCANCODE_Q: chip8.keys[0x4] = pressed; break;
        case SDL_SCANCODE_W: chip8.keys[0x5] = pressed; break;
        case SDL_SCANCODE_E: chip8.keys[0x6] = pressed; break;
        case SDL_SCANCODE_R: chip8.keys[0xD] = pressed; break;

        case SDL_SCANCODE_A: chip8.keys[0x7] = pressed; break;
        case SDL_SCANCODE_S: chip8.keys[0x8] = pressed; break;
        case SDL_SCANCODE_D: chip8.keys[0x9] = pressed; break;
        case SDL_SCANCODE_F: chip8.keys[0xE] = pressed; break;

        case SDL_SCANCODE_Z: chip8.keys[0xA] = pressed; break;
        case SDL_SCANCODE_X: chip8.keys[0x0] = pressed; break;
        case SDL_SCANCODE_C: chip8.keys[0xB] = pressed; break;
        case SDL_SCANCODE_V: chip8.keys[0xF] = pressed; break;
    }
}

void draw()
{
	uint32_t pixels[64 * 32];
	unsigned int x, y;
		
	if (chip8.drawflag)
	{
		memset(pixels, 0, (64 * 32) * 4);
		for(x=0;x<64;x++)
		{
			for(y=0;y<32;y++)
			{
				if (chip8.gfx[(x) + ((y) * 64)] == 1)
				{
					pixels[(x) + ((y) * 64)] = UINT32_MAX;
				}
			}
		}
		
		SDL_UpdateTexture(screen, NULL, pixels, 64 * sizeof(uint32_t));
	
		SDL_Rect position;
		position.x = 0;
		position.y = 0;
		// If you change SDL_RenderSetLogicalSize, change this accordingly.
		position.w = 64;
		position.h = 32;
		SDL_RenderCopy(renderer, screen, NULL, &position);
		SDL_RenderPresent(renderer);
	}
	chip8.drawflag = 0;
}


void Instruction(uint8_t byte1, uint8_t byte2){
    uint8_t nibble1 = (byte1 & 0xF0) >> 4; // premier nibble (bits 12-15)
    uint8_t nibble2 = byte1 & 0x0F;        // deuxième nibble (bits 8-11)
    uint8_t nibble3 = (byte2 & 0xF0) >> 4; // troisième nibble (bits 4-7)
    uint8_t nibble4 = byte2 & 0x0F;        // quatrième nibble (bits 0-3
    switch (nibble1)
    {
    case 0x6:
        uint8_t lastNibble = byte1 & 0x0F;
        chip8.V[lastNibble] = byte2;
        printf("\033[32mLD V[%X], %02X\033[0m", lastNibble, byte2);
        break;
    case 0xA:
        uint16_t adr = (nibble2<<8) | byte2;
        chip8.I = adr;
        printf("\033[32mLD I, %03X\033[0m", adr);
        break;
    case 0x2:
        uint16_t addr = (nibble2<<8)|byte2;
        chip8.SP++;
        chip8.stack[chip8.SP] = chip8.PC;
        chip8.PC = addr;
        printf("\033[32mCALL %X\033[0m",addr);
        break;
    case 0x0:
        if (byte1 == 0x00 && byte2 == 0xEE){
            uint16_t addr = chip8.stack[chip8.SP];
            chip8.PC = addr;
            chip8.SP--;
            printf("\033[32mRET %X\033[0m",addr);
        }if (byte1 == 0x00 && byte2 == 0xE0){
            //clear display
            for (int pixelCounter = 0; pixelCounter < (64*32); pixelCounter++){
                chip8.gfx[pixelCounter] = 0;
            }
            chip8.drawflag = 1;
            printf("\033[32mCLS\033[0m");
        }
        break;
    case 0x1:
        //jump to nnn
        chip8.PC = (nibble2<<8)|byte2;
        printf("\033[32mJP %X\033[0m",(nibble2<<8)|byte2);           
        break;
    case 0x3:
        if (chip8.V[nibble2] == byte2){
            chip8.PC += 2;
            
        }
        printf("\033[32mSE V[%X] %02X\033[0m", nibble2, byte2); 
        break;
    case 0xF:
        if (byte2 == 0x07){
            chip8.V[nibble2] = chip8.delay;
            printf("\033[32mLD V[%X] %X\033[0m",nibble2,chip8.delay); 
        }
        if (byte2 == 0x15){
            chip8.delay = chip8.V[nibble2];
            printf("\033[32mLD %X V[%X]\033[0m",chip8.delay,nibble2); 
        }
        if (byte2 == 0x18){
            chip8.sound = chip8.V[nibble2];
            printf("\033[32mLD ST V[%X]\033[0m",nibble2); 
        }
        if (byte2 == 0x33){
            uint8_t value = chip8.V[nibble2];
            chip8.memory[chip8.I] = value / 100;
            chip8.memory[chip8.I + 1] = (value / 10) % 10;
            chip8.memory[chip8.I + 2] = value % 10;
            printf("\033[32mLD BCD V[%X]\033[0m",nibble2);
        }
        if (byte2 == 0x65){
            for (int i = 0; i <= nibble2; i++){
                chip8.V[i] = chip8.memory[chip8.I + i];
            }
            printf("\033[32mLD V[0] to V[%X], [I]\033[0m",nibble2);
        }if (byte2 == 0x29){
            chip8.I = 0x50 + chip8.V[nibble2] * 5; // chaque sprite fait 5 octets
            printf("\033[32mLD F, V[%X]\033[0m",nibble2);
        }
        break;
    case 0x4:
        if (chip8.V[nibble2] != byte2){
            chip8.PC += 2;
        }
        printf("\033[32mSNE V[%X] %X\033[0m",nibble2,byte2);
    case 0x7:   
        chip8.V[nibble2] = chip8.V[nibble2] + byte2;
        printf("\033[32mADD V[%X] %X\033[0m",nibble2,byte2);
    case 0x8:
        if (nibble4 == 0x0){
            chip8.V[nibble2] = chip8.V[nibble3];
            printf("\033[32mLD V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x1){
            chip8.V[nibble2] |= chip8.V[nibble3]; // or mais en raccourcis
            printf("\033[32mOR V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x2){
            chip8.V[nibble2] &= chip8.V[nibble3]; // et mais en raccourcis
            printf("\033[32mAND V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x3){
            chip8.V[nibble2] ^= chip8.V[nibble3]; //Xor mais en raccourcis
            printf("\033[32mXOR V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x4){
            uint16_t sum = chip8.V[nibble2] + chip8.V[nibble3];
            if (sum > 0xFF){
                chip8.V[15] = 1;
            }else{
                chip8.V[15] = 0;
            }
                
            chip8.V[nibble2] = sum & 0xFF; //AND mais avec une retenu
            
            
            printf("\033[32mADD V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x5){
            if (chip8.V[nibble2]>chip8.V[nibble3]){
                chip8.V[15] = 1;
            }else{
                chip8.V[15] = 0;
            }
            chip8.V[nibble2] -= chip8.V[nibble3];
            printf("\033[32mSUB V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x6){
            uint16_t opcode = (byte1 << 8) | byte2;
            uint8_t x = (opcode & 0x0F00) >> 8;
            chip8.V[0xF] = chip8.V[nibble2] & 0x1;
            chip8.V[nibble2] >>= 1; //decale les bits vers la droite
            printf("\033[32mSHR V[%X] {, V[%X]}\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0x7){
            if (chip8.V[nibble2]<chip8.V[nibble3]){
                chip8.V[0xF] = 1;
            }else{
                chip8.V[0xF] = 0;
            }
            chip8.V[nibble3] -= chip8.V[nibble2];
            printf("\033[32mSUBN V[%X] V[%X]\033[0m",nibble2,nibble3);
        }
        if (nibble4 == 0xE){
            uint16_t opcode = (byte1 << 8) | byte2;
            uint8_t x = (opcode & 0x0F00) >> 8;

            chip8.V[0xF] = (chip8.V[x] & 0x80) >> 7; // récupérer MSB dans VF
            chip8.V[x] <<= 1;                          // décalage à gauche
            printf("\033[32mSHL V[%X] {, V[%X]}\033[0m",nibble2,nibble3);
        }
            
        break;
    case 0xC:
        uint8_t randomValue = rand() % 256; // Valeur aléatoire entre 0 et 255
        chip8.V[nibble2] = randomValue & byte2;
        printf("\033[32mRND V[%X], %02X\033[0m", nibble2, byte2);
        break;
    case 0xD:
    
        chip8.V[0xF] = 0; // VF = 0
        for (int row = 0; row < nibble4; row++) {
            uint8_t spriteByte = chip8.memory[chip8.I + row];
            for (int col = 0; col < 8; col++) {
                uint8_t spritePixel = (spriteByte >> (7 - col)) & 1;
                int x = (chip8.V[nibble2] + col) % 64;
                int y = (chip8.V[nibble3] + row) % 32;

                if (spritePixel) {
                    if (chip8.gfx[y * 64 + x] == 1)
                        chip8.V[0xF] = 1; // collision
                    chip8.gfx[y * 64 + x] ^= 1; // XOR
                }
            }
        }
        chip8.drawflag = 1;
        printf("\033[32mDRW V[%X] v[%X], %X\033[0m",nibble2,nibble3,nibble4);
        break;
    case 0xE: 
        uint16_t opcode = (byte1 << 8) | byte2;
        uint8_t x = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF) {
            case 0x9E: // SKP Vx
                if (chip8.keys[ chip8.V[x] ]) chip8.PC += 2;
                printf("\033[32mSKP V[%X] %X\033[0m",x,chip8.keys[chip8.V[x]]);
                break;
            case 0xA1: // SKNP Vx
                if (!chip8.keys[ chip8.V[x] ]) chip8.PC += 2;
                printf("\033[32mSKNP V[%X] %X\033[0m",x,chip8.keys[chip8.V[x]]);
                break;
        }
        break;
    
    default:
        printf("\033[31mNOT DEFINED\033[0m");
        break;
    }

    
}


void run_chip8(){
    chip8.PC = 0x200; // Program counter starts at 0x200
    chip8.I = 0; // Reset index register
    chip8.SP = 0; // Reset stack pointer
    chip8.delay = 0; // Reset delay timer
    chip8.sound = 0; // Reset sound timer
    chip8.drawflag = 0;
    for (int i = 0; i < 16; i++) chip8.keys[i] = 0; // reset
    for (int i = 0; i < 80; ++i) {
        chip8.memory[0x50 + i] = chip8_fontset[i];
    }
    int running = 1;
    while(running){
        STARTtimer = clock();
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            handle_keyboard(event);
        }

        

        uint16_t opcode = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC+1];
        uint8_t byte1 = (opcode >> (8*1)) & 0xff; //octet de poids fort
        uint8_t byte2 = (opcode >> (8*0)) & 0xff; //octet de poids faible

        printf(" %02X%02X ", byte1,byte2);
        chip8.PC += 0x002;
        Instruction(byte1, byte2);
        draw();
        update_timer();
        ENDtimer = clock();
        double elapsed_ms = ((double)(ENDtimer - STARTtimer) / CLOCKS_PER_SEC) * 1000.0;
        if (16666 - elapsed_ms > 0){
            usleep(16666 -elapsed_ms); //16 ms
        }
        
        
        printf("\n"); // revient au début de l'écran du terminal


    }
}

int main(int argc, char const *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    int W = 1024, H = 512;
    if (!argv[1]){
        printf("merci de preciser un fichier, ./main nom_du_fichier.ch8\n");
        return 0;
    }

    win = SDL_CreateWindow("Chip8 Emulateur",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,W,H,0);
    renderer = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);
    SDL_RenderSetLogicalSize(renderer, 64, 32);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);	
    STARTtimer = clock();
    SDL_UpdateWindowSurface(win);

    load_cartouche_in_memory(argv[1]);
    run_chip8();

    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
}
