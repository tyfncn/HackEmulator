#include <stdio.h>
#include <time.h>
#include "gfx.h"

#define DEBUGF 0
#define DEBUGE 0
#define DEBUGR 0
#define DEBUGO 0
#define DEBUGI 0

//1111110010101000
int loadSource(char *);
int InstructionFetchDecode();
int InstructionExecuteWriteBack();
int updateIO();


//Memory Components
unsigned short ROM[32768];   // 32KB rom
short RAM[24577];   // 24KB ram
short RegA;
short RegD;
unsigned short RegPC;

//helpers
void printfBinary(const char*, unsigned short num);
short updatedPixel=0;
short insLMB,insValue,insA,insComp,insDest,insJump;

int main(int argc, char** argv){
  	if (argc < 2) 
  		{printf("1 program ver de calistirak!\n");return 1;}
  	if ( loadSource(argv[1]) != 0)
  		{printf("Error on loading source!\n");return 1;}

	gfx_open(512,256,"Example Graphics Program");
    gfx_clear_color(100,100,100);
    gfx_clear();
    
    while (1){
    	InstructionFetchDecode();
    	InstructionExecuteWriteBack();
    	updateIO();
//    	nanosleep(100000000);
    }


	return 0;
}



int updateIO(){
	char c;
	if (gfx_event_waiting()){
		c = gfx_wait();
		switch(c){
			case  13: RAM[24576]=128; break;
			case   8: RAM[24576]=129; break;
			case  81: RAM[24576]=130; break;
			case  82: RAM[24576]=131; break;
			case  83: RAM[24576]=132; break;
			case  84: RAM[24576]=133; break;
			case  80: RAM[24576]=134; break;
			case  87: RAM[24576]=135; break;
			case  85: RAM[24576]=136; break;
			case  86: RAM[24576]=137; break;
			case  99: RAM[24576]=138; break;
			case  -1: RAM[24576]=139; break;
			case  27: RAM[24576]=140; break;
			case -66: RAM[24576]=141; break;
			case -65: RAM[24576]=142; break;
			case -64: RAM[24576]=143; break;
			case -63: RAM[24576]=144; break;
			case -62: RAM[24576]=145; break;
			case -61: RAM[24576]=146; break;
			case -60: RAM[24576]=147; break;
			case -59: RAM[24576]=148; break;
			case -58: RAM[24576]=149; break;
			case -57: RAM[24576]=150; break;
			case -56: RAM[24576]=151; break;
			case -55: RAM[24576]=152; break;
			default:
			RAM[24576]=c;
		}
		// if (DEBUGI) printf("%c (%d)\n", RAM[24576],RAM[24576]);
	}
	if (updatedPixel!=0){
//		printf("\nScreenUpdate! at :%d,%d\n",(updatedPixel-16384)%32,(updatedPixel-16384)/32);
		updatedPixel=updatedPixel-16384;
		short k,c;
		for (c = 15; c >= 0; c--)
		  {
		    k = RAM[updatedPixel+16384] >> c;
		 
		    if (k & 1){
				gfx_color(0,0,0);
				// if (DEBUGO) printf("B:%d,%d\n", 16*(updatedPixel%32)+c, updatedPixel/32);
		    }
		    else{
				gfx_color(254,254,254);		
				// if (DEBUGO) printf("W:%d,%d\n", 16*(updatedPixel%32)+c, updatedPixel/32);
		    }
			gfx_point(16*(updatedPixel%32)+c, updatedPixel/32);
		  }
		// if (DEBUGO) printf("C:%d\n", updatedPixel);
		updatedPixel=0;
		gfx_flush();
	}

}


int InstructionFetchDecode(){
	short instruction = ROM[RegPC];
	/*
	1110000000000000 -> 57344 LMB
	0111111111111111 -> 32767 Value (a-type)
	0000111111000000 -> 4032  Comp (c-type)
	0000000000111000 -> 56    Dest (c-type)
	0000000000000111 -> 7     Jump (c-type)
	*/
	insLMB = (instruction & 57344) >> 13;
	insValue = instruction & 32767;
	insA = (instruction &4096 )>>12;
	insComp = (instruction & 4032) >> 6;
	insDest = (instruction & 56) >> 3;
	insJump = instruction & 7;

  //   	if (DEBUGF){
		// 	printf(">Instruction Fect & Decode\n");
		// 	printfBinary(" Instruction",instruction);
		// 	printfBinary(" Ins type",insLMB);
		// 	printfBinary(" @Value",insValue);
		// 	printfBinary(" A",insA);
		// 	printfBinary(" Comp",insComp);
		// 	printfBinary(" Dest",insDest);
		// 	printfBinary(" Jump",insJump);
		// }
}

int InstructionExecuteWriteBack(){
	short executed=0;
	short RegAprevious = RegA;

	switch(insLMB){
		case 0:
		case 1:
		case 2:
		case 3:	//a-type
			RegA = insValue;
			RegPC++;
			// if(DEBUGE){
			// 	printf(">Executing A-TYPE with Value=");
			// 	printfBinary(" Value",insValue);
			// }
		break;
		case 7: //c-type
			// if(DEBUGE){
			// 	printf(">Executing C-TYPE with\n");
			// 	printfBinary(" A",insA);
			// 	printfBinary(" Comp",insComp);
			// 	printfBinary(" Dest",insDest);
			// 	printfBinary(" Jump",insJump);
			// }
			switch(insComp){
				case 42: //101010 -> 0 (a=0)
					executed = 0;
				break; 
				case 63: //111111 -> 1 (a=0)
					executed = 1;
				break;
				case 58: //111010 -> -1 (a=0) 
					executed=-1;
				break;
				case 12: //001100 -> D (a=0)
					executed = RegD;
				break;
				case 48: //110000 -> A (a=0), M (a=1)
					if (insA){
						executed=RAM[RegA];
					}else{
						executed=RegA;
					}
				break;
				case 13: //001101 -> !D (a=0)
					executed = ~RegD;
				break;
				case 49: //110001 -> !A (a=0), !M (a=1)
					if (insA){
						executed=~RAM[RegA];
					}else{
						executed=~RegA;
					}
				break;
				case 15: //001111 -> -D (a=0)
					executed = -RegD;
				break;
				case 51: //110011 -> -A (a=0), -m (a=1)
					if (insA){
						executed=-RAM[RegA];
					}else{
						executed=-RegA;
					}
				break;
				case 31: //011111 -> D+1
					executed = RegD + 1;
				break;
				case 55: //110111 -> A+1 (a=0), M+1 (a=1)
					if (insA){
						executed=RAM[RegA]+1;
					}else{
						executed=RegA+1;
					}
				break;
				case 14: //001110 -> D-1 (a=0)
					executed = RegD-1;
				break;
				case 50: //110010 -> A-1 (a=0), M-1 (a=1)
					if (insA){
						executed=RAM[RegA]-1;
					}else{
						executed=RegA-1;
					}
				break;
				case  2: //000010 -> D+A (a=0), D+M (a=1)
					if (insA){
						executed=RAM[RegA]+RegD;
					}else{
						executed=RegA+RegD;
					}
				break;
				case 19: //010011 -> D-A (a=0), D-M (a=1)
					if (insA){
						executed= RegD - RAM[RegA];
					}else{
						executed= RegD - RegA;
					}
				break;
				case  7: //000111 -> A-D (a=0), M-D (a=1)
					if (insA){
						executed= RAM[RegA] - RegD;
					}else{
						executed= RegA - RegD;
					}
				break;
				case  0: //000000 -> D&A (a=0), D&M (a=1)
					if (insA){
						executed=RAM[RegA] & RegD;
					}else{
						executed=RegA & RegD;
					}
				break;
				case 21: //010101 -> D|A (a=0), D|M (a=1)
					if (insA){
						executed = RegD | RAM[RegA];
					}else{
						executed = RegD | RegA;
					}
				break;
				default:
					printfBinary("Invalid Comp value",insComp);
			}
			// if (DEBUGE){
			// 	printfBinary(">Executed:",executed);
			// }
			switch(insDest){
				case 0: // 000 null

				break;
				case 1: // 001 M
					RAM[RegA] = executed;
					if (RegA >= 16384){updatedPixel=RegA;}
				break;
				case 2: // 010 D
					RegD = executed;
				break;
				case 3: // 011 MD
					RAM[RegA] = executed;
					RegD = executed;
					if (RegA >= 16384){updatedPixel=RegA;}
				break;
				case 4: // 100 A
					RegA = executed;
				break;
				case 5: // 101 AM
					RAM[RegA] = executed;
					RegA = executed;
					if (RegA >= 16384){updatedPixel=RegA;}
				break;
				case 6: // 110 AD
					RegA = executed;
					RegD = executed;
				break;
				case 7: // 111 AMD
					RAM[RegA] = executed;
					RegA = executed;
					RegD = executed;
					if (RegA >= 16384){updatedPixel=RegA;}
				break;
				default:
					printfBinary("Invalid dest value:",insDest);
					return 1;
			}

			switch(insJump){
				case 0: //000 null
					RegPC = RegPC+1;
				break;
				case 1: //001 JGT
					if(executed > 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 2: //010 JEQ
					if(executed == 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 3: //011 JGE
					if(executed >= 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 4: //100 JLT
					if(executed < 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 5: //101 JNE
					if(executed != 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 6: //110 JLE
					if(executed <= 0){
						RegPC = RegAprevious;
					}else{RegPC = RegPC+1;}
				break;
				case 7: //111 JMP
					RegPC = RegAprevious;
				break;
				default:
					printfBinary("Invalid jump value:",insDest);
					return 1;
			}
		break;
		default:
		printf("Unknown instruction %d\n", ROM[RegPC]);
		printfBinary("binary:",ROM[RegPC]);
	}

	return 0;
}

int loadSource(char *filename){
	FILE *fp = fopen(filename, "r");
	if (!fp) {printf("File Read Error!\n");return 2;}

	char buf[17];
	unsigned short rom_counter = 0;
	while ( fread(buf, 1, sizeof(buf), fp) ){
   		int i=0;
   		unsigned short ins = 0;
   		for (i=0; i<16; i++){
   			ins = ins*2 + (buf[i] - 48);
   		}
   		ROM[rom_counter++] = ins;
   	}
   	fclose(fp);
   	ROM[rom_counter]=-1;
   	return 0;
}

void printfBinary(const char* message, unsigned short num){
	printf("%s: ", message);
	short k,c;
	for (c = 15; c >= 0; c--)
	  {
	    k = num >> c;
	    if (k & 1)
	      printf("1");
	    else
	      printf("0");
	  }
	printf("\n");	  
}
