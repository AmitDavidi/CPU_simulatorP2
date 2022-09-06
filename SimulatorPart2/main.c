#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IMEMIN_NUMBER_OF_LINES 4096
#define IMEMIN_LINE_SIZE 12
#define DMEMIN_NUMBER_OF_LINES 4096
#define DMEMIN_LINE_SIZE 8

#define   irq0enable     IORegister[0] 
#define   irq1enable     IORegister[1]
#define   irq2enable     IORegister[2]
#define   irq0status     IORegister[3] 
#define   irq1status     IORegister[4]
#define   irq2status     IORegister[5]
#define   irqhandler     IORegister[6]
#define   irqreturn      IORegister[7]
#define   clks           IORegister[8]
#define   ledsREG        IORegister[9]
#define	  display7segREG IORegister[10]
#define	  timerenable    IORegister[11]
#define	  timercurrent   IORegister[12]
#define	  timermax       IORegister[13]
#define	  diskcmd        IORegister[14]
#define	  disksector     IORegister[15]
#define	  diskbuffer     IORegister[16]
#define	  diskstatus     IORegister[17]
#define	  reserved1      IORegister[18]
#define	  reserved2      IORegister[19]
#define	  monitoraddr    IORegister[20]
#define	  monitordata    IORegister[21]
#define	  monitorcmd     IORegister[22]


int srlLogicalShift(int number, int shift_val) {
	int result = number >> shift_val;
	unsigned mask = (~(1 << 31));
	for (int i = 0; i < shift_val; i++)
	{
		result &= mask;
		mask = mask >> 1;
	}
	return result;

}
// gets an instruction and decodes it.
// changes relevant values in registers and opcode.
void decode(int* opcode, int* rd, int* rs, int* rt, int* rm, char* instruction, int *R) {
	char buffer[4] = { 0 };

	buffer[0] = instruction[0];
	buffer[1] = instruction[1];
	buffer[2] = 0;
	*opcode = strtol(buffer, NULL, 16); // translate first two chars to int - opcode
	
	buffer[0] = instruction[2];
	buffer[1] = 0;
	*rd = strtol(buffer, NULL, 16); // translate third char to int

	buffer[0] = instruction[3];
	buffer[1] = 0;

	*rs = strtol(buffer, NULL, 16); // translate third char to int

	buffer[0] = instruction[4];
	buffer[1] = 0;
	*rt = strtol(buffer, NULL, 16); // translate fourth char to int

	buffer[0] = instruction[5];
	buffer[1] = 0;
	*rm = strtol(buffer, NULL, 16); // translate fifth char to int

	buffer[0] = instruction[6];
	buffer[1] = instruction[7];
	buffer[2] = instruction[8];
	buffer[3] = 0;
	R[1] = strtol(buffer, NULL, 16);



	buffer[0] = instruction[9];
	buffer[1] = instruction[10];
	buffer[2] = instruction[11];
	buffer[3] = 0;
	R[2] = strtol(buffer, NULL, 16);



}

// returns 1 if halt
// executes the command based on opcode
int execute(int opcode, int rd, int rs, int rt, int rm,  int* IORegister, int* R, int* pc, int* MEM, int *handling_interrupt) {
	//
	printf("Executing inst: %d,  code: %d\n", *pc, opcode);
	// write to trace.txt


	// execute opcode 

	// instructions that writes into R[0], R[1] R[2] Does not change value
	switch (opcode)
	{

	case(0):  //add
		if (rd > 2)
			R[rd] = R[rs] + R[rt] + R[rm];
		(*pc)++;
		return 0;
			
	case(1):  // sub
		if (rd > 2)
			R[rd] = R[rs] - R[rt] - R[rm];
		(*pc)++;
		return 0;


	case(2):  // mac
		if (rd > 2)
			R[rd] = R[rs] * R[rt] + R[rm];
		(*pc)++;
		return 0;

	case(3): // and
		if (rd > 2)
			R[rd] = R[rs] & R[rt] & R[rm];
		(*pc)++;
		return 0;

	case(4): // or
		if (rd > 2)
			R[rd] = R[rs] | R[rt] | R[rm];
		(*pc)++;
		return 0;

	case(5): // xor
		if (rd > 2)
			R[rd] = R[rs] ^ R[rt] ^ R[rm];
		(*pc)++;
		return 0;

	case(6): // sll
		if (rd > 2)
			R[rd] = R[rs] << R[rt];
		(*pc)++;
		return 0;

	case(7): // sra
		if (rd > 2)
			R[rd] = (R[rs] >> R[rt]);
		(*pc)++;
		return 0;

	case(8): // srl
		if (rd > 2)
			R[rd] = srlLogicalShift(R[rs], R[rt]);
		(*pc)++;
		return 0;


	case(9): // beq
		if (R[rs] == R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]
		
		else 
			(*pc)++;
		return 0;


	case(10): // bne
		if (R[rs] != R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]

		else 
			(*pc)++;
		return 0;


	case(11): // blt
		if (R[rs] < R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]
		
		else
			(*pc)++;
		return 0;


	case(12): // bgt
		if (R[rs] > R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]
		
		else 
			(*pc)++;
		return 0;

	case(13): // ble
		if (R[rs] <= R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]

		else 
			(*pc)++;
		return 0;


	case(14): // bge
		if (R[rs] >= R[rt])
			(*pc) = R[rm] & 0x00000FFF; // only first 12 bits of R[rm]

		else 
			(*pc)++;

		return 0;


	case(15): // jal
		if (rd > 2) {
			R[rd] = (unsigned int)((*pc) + 1);
			(*pc) = (unsigned int)(R[rm] & 0x00000FFF);
		}
		else 
			(*pc)++;
		return 0;


	case(16): // lw
		if (rd > 2)
			R[rd] = MEM[R[rs] + R[rt]] + R[rm];
		(*pc)++;
		return 0;


	case(17): // sw
		MEM[R[rs] + R[rt]] = R[rm] + R[rd];
		(*pc)++;
		return 0;



	case(18): // reti
		(*pc) = IORegister[7];
		(*handling_interrupt) = 0;
		IORegister[3] = 0;
		IORegister[4] = 0;
		IORegister[5] = 0;

		return 0;


	case(19): // in
		if (R[rs] + R[rt] == 22) 
			R[rd] = 0;

		else {
			if (rd > 2)
				R[rd] = IORegister[R[rs] + R[rt]];
		}

		(*pc)++;
		return 0;


	case(20): // out
		IORegister[R[rs] + R[rt]] = R[rm];

		(*pc)++;
		return 0;


	case(21): // halt
		printf("Halting...\n");
		return 1;

	}

}

// strips a string of its new line character ( if exists)
void snip_nl(char* string) {
	int i = 0;
	while (string[i] != '\n') {
		if (string[i] == 0)
			return;
		i++;
	}
	string[i] = 0;
}


// this function gets a source file filled with hex:strings
// updates a result array with those strings.
// fills the rest with zeros
void store_file_into_integer_array(FILE* source_file, unsigned int* result_array, int arraySize, int bufferLength)
{
	char* buffer = (char*)malloc((1 + bufferLength) * sizeof(char));
	int i = 0;
	while (fgets(buffer, bufferLength + 1, source_file))
		result_array[i++] = strtol(buffer, NULL, 16);

	while (i < arraySize) // zero out the rest of the memory
		result_array[i++] = 0;

	free(buffer);
}


// this function gets a source file filled with hex:strings
// updates a result array with the integer value of those strings.
// fills the rest with zeros
void store_file_into_string_array(FILE* source_file, char** result_array, const int arraySize, const int bufferLength)
{
	// bufferLength + 2 because New line --
	char* buffer = (char*)malloc(bufferLength + 2);
	int idx = 0;
	
	while (fgets(buffer, bufferLength + 2, source_file)) { 

		snip_nl(buffer); // remove newline from buffer \n

		result_array[idx] = (char*)malloc(bufferLength + 1); // allocate memory
		
		if (result_array[idx] != 0) // catch memory allocation fail
			strcpy(result_array[idx], buffer);

		else
		{
			printf("Error allocating memory...");
			exit(1);
		}
		idx++;
	}

	

	// store strings of zeros after the last line
	for (int i = 0; i < bufferLength; i++)
		buffer[i] = '0';
	buffer[bufferLength] = 0;

	while (idx < arraySize) {

		result_array[idx] = (char*)malloc(bufferLength + 1); // allocate memory

		if (result_array[idx] != 0) // catch memory allocation fail
			strcpy(result_array[idx], buffer); // copy 000000000000\0 

		else
		{
			printf("Error allocating memory...");
			exit(1);
		}

		idx++;
	}

	free(buffer);
	/*
	for (int i = 0; i < arraySize; i++) {
		free(result_array[i]);
		printf("reached %d\n", i);
	}

	printf("All good !!");
	*/
}






int main(int argc, char* argv[]) {
	if (argc < 15) {
		printf("Input incomplete");
		exit(1);
	}

	//clock_t start, end;
	//start = clock();
	// end = clock();
	// printf("%f", (double)(end - start) / CLOCKS_PER_SEC);


	// -- Load and create files --

	// open files
	FILE* imemin = fopen(argv[1], "r");
	FILE* dmemin = fopen(argv[2], "r");
	FILE* diskin = fopen(argv[3], "r");
	FILE* irq2in = fopen(argv[4], "r");
	FILE* dmemout = fopen(argv[5], "wb+");
	FILE* regout = fopen(argv[6], "wb+");
	FILE* trace = fopen(argv[7], "wb+");
	FILE* hwregtrace = fopen(argv[8], "wb+");
	FILE* cycles = fopen(argv[9], "wb+");
	FILE* leds = fopen(argv[10], "wb+");
	FILE* display7seg = fopen(argv[11], "wb+");
	FILE* diskout = fopen(argv[12], "wb+");
	FILE* monitor = fopen(argv[13], "wb+");
	FILE* monitor_yuv = fopen(argv[14], "wb+");
	FILE* files[14] = { imemin, dmemin, diskin, irq2in, dmemout, regout, trace, hwregtrace, cycles, leds, display7seg, diskout, monitor, monitor_yuv };

	for (int k = 0; k < 14; k++) {
		if (files[k] == NULL) {
			printf("Error loading file %d...", k);
			exit(1);
		}
	}

	// iterate over instructions store inside an array

	char** instructions = (char**)malloc(IMEMIN_NUMBER_OF_LINES * sizeof(char*));
	store_file_into_string_array(imemin, instructions, IMEMIN_NUMBER_OF_LINES, IMEMIN_LINE_SIZE);

	// store RAM inside array
	int* MEM = (int*)calloc(DMEMIN_NUMBER_OF_LINES , sizeof(int));
	store_file_into_integer_array(dmemin, MEM, DMEMIN_NUMBER_OF_LINES, DMEMIN_LINE_SIZE);



	int SIMPRegisters[16] = { 0 };
	int PC = 0;
	int halt_flag = 0;
	int opcode = 0;
	int rd = 0, rs = 0, rt = 0, rm = 0;
	unsigned int IORegister[23] = { 0 };

	char* current_instruction = (char*)malloc(IMEMIN_LINE_SIZE + 1);
	int handling_interrupt = 0;
	int disk_writing_timer = 0;


	// FDE procedure
	while (halt_flag != 1) {
		// handle clock
		if (timercurrent == timermax) {
			irq0status = 1;
			timercurrent = 0;
		}
		else if (timerenable) {
			timercurrent++;
		}

		if (timercurrent == 100 || timercurrent == 200 || timercurrent == 300 || timercurrent == 400 || timercurrent == 500 || timercurrent == 600 || timercurrent == 700 || timercurrent == 800)
			irq2enable = 1;

		// handle interrupts
		if (handling_interrupt == 0 && (irq0enable & irq0status | irq1enable & irq2status | irq2enable & irq2status)) {
			handling_interrupt = 1;
			irqreturn = PC; // opcode reti sets <PC> back to be <irqreturn> and <handling_ruling> to 0
			PC = irqhandler;
		}

		// fetch decode execute
		strcpy(current_instruction, instructions[PC]); // fetch

		decode(&opcode, &rd, &rs, &rt, &rm, current_instruction, SIMPRegisters); // decode

		halt_flag = execute(opcode, rd, rs, rt, rm, IORegister, SIMPRegisters, &PC, MEM, &handling_interrupt); // execute

		// done with this clock cycle
		// handle hard disk
		if (diskstatus == 1) {
			disk_writing_timer--; // when writing we set disk_writing_timer to 1024
			if (disk_writing_timer == 0)
			{
				diskcmd = 0;
				diskstatus = 0;
				irq1status = 1;

			}
		}

	}




	// free instructions
	for (int k = 0; k < IMEMIN_NUMBER_OF_LINES; k++)
		free(instructions[k]);

	free(instructions);
	free(current_instruction);
	free(MEM);

	// close files
	for (int k = 0; k < 14; k++)
		fclose(files[k]);
	return 0;

}
