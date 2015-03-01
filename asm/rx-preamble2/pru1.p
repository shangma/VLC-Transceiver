.origin 0
.entrypoint INIT

#define OFFSET_LIM 16777208//16376 // maximum byte offset
#define DDR_ADDRESS 0x90000000
#define READY_CODE 0xaa
#define DONE_CODE 0xff
#define PACKS_2_RCV 15
#include "../../include/asm.hp"

INIT:
	// Enable OCP master port
    	LBCO      r0, C4, 4, 4
    	CLR       r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    	SBCO      r0, C4, 4, 4

        MOV     r0, 0x00000120      // Configure the programmable pointer register for PRU0 by setting c28_pointer[15:0]
	MOV     r1, PRU1CTPPR_0         // field to 0x0120.  This will make C28 point to 0x00012000 (PRU shared RAM).
	SBBO    r0, r1, 0, 4
	
	MOV		  r2, DDR_ADDRESS	
	MOV		  r0, 8 // init write offset to ninth byte
	SBBO	  r0, r2, 4, 4 // write initial offset to RAM
	MOV		  r1, OFFSET_LIM // store end of buffer location
	MOV		  r4.b0, 0
	MOV		  r4.b1, DONE_CODE
	MOV		  r4.b2, READY_CODE
	SBCO      r4.b0, CONST_PRUSHAREDRAM, 0, 1 // make sure packet ready on init is not true
	MOV r5, 0


POLL_PX: // poll until PRU0 says a packet is ready
	LBCO r5.b0, CONST_PRUSHAREDRAM, 0, 1 
	QBNE POLL_PX, r5.b0, r4.b2

READ_DATA: // pull in data from PRU0
	XIN 10, r8, 88 // read from SP
	
	// if overflow condition not met
	//QBNE STORE_DATA, r0, r1 // jump to STORE_DATA

	// else
	//MOV r0, 8 // reset offset to start of buffer

STORE_DATA:
	SBBO r8, r2, r0, 88 // push packet to RAM
	ADD r0, r0, 176 // increment offset
	SBBO r0, r2, 4, 4 // store write offset to RAM

	SBCO r4.b0, CONST_PRUSHAREDRAM, 0, 1 // overwrite the packet ready code

	ADD r5, r5, 1
	QBEQ STOP, r5, PACKS_2_RCV
	//LBBO r5.b0, r2, 0, 1 // check the value of the done code
	// if host has written done code
	//QBEQ STOP, r5.b0, r4.b1 // jump to STOP and halt operation

	// else
	JMP POLL_PX // return to main loop

STOP:
	MOV r31.b0, PRU1_ARM_INTERRUPT + 16 // send program complete interrupt to host
	HALT // shut down microcontroller