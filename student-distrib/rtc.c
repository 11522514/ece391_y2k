/* rtc.c - Implements the rtc driver and handler
 */

#include "rtc.h"
#include "i8259.h"
#include "lib.h"

//flag to indicate interrupt occured for RTC
volatile int interrupt_flag = 0;

/*
*	rtc_intialize
*	DESCRIPTION:	initialize the RTC with set freq = 2Hz
*	INPUT:			None
*	OUTPUT:			None
*	RETURN VALUE:	None
*	SIDE EFFECT:	sets the freq of RTC to 2Hz
*
*/
uint32_t rtc_initialize()
{
	cli();										// masks the interrupts
	outb(inb(INDEX_PORT) | 0x80, INDEX_PORT);	// disables Non-maskable interrupts

	outb(STATUS_B, INDEX_PORT);					// set the index to register B
	char prev = inb(DATA_PORT);					// read the current value of register B
	outb(STATUS_B, INDEX_PORT);					//set index again
	outb(prev | 0x40, DATA_PORT);				//enable bit 6 of register B
	setFreq(1024);									// call function to set the frq = 2Hz
	enable_irq(0x08);

	outb(inb(INDEX_PORT) & 0x7F, INDEX_PORT);	// enable Non-maskable interrupts
	sti();										// unmask the interrupts
	return 0;
}

/*
*	rtc_int_handler
*	DESCRIPTION:	the interrupt handler for RTC
*	INPUT:			Void
*	OUTPUT:			None
*	RETURN VALUE:	None
*	SIDE EFFECT:	Handles the interrupts for RTC
*
*/
void rtc_int_handler(void)						// simple RTC for Checkpoint 1
{
	cli();
	outb(STATUS_C, INDEX_PORT);
	inb(DATA_PORT);
	send_eoi(8);

	interrupt_flag = 1;

	sti();
}

/*
*	setFreq
*	DESCRIPTION:	sets the RTC to the specfied freq
*	INPUT:			the frequency to set the RTC to
*	OUTPUT:			None
*	RETURN VALUE:	-1 on Fail, 0 on Success
*	SIDE EFFECT:	sets the freq of RTC to the frequency passed into the function
*
*/
uint32_t setFreq(int32_t freq)
{
	// check to see if the new freq is within range of possible freq
	if(freq <= 2 || 1024 <= freq)
	{
		return -1;								// if not, return fail
	}

	if(!isPowerOfTwo(freq))
	{
		return -1;
	}

	// calculate the rate 
	int rate = 0;
	int x = 32768 / freq;
	while(x != 0)
	{
		rate = rate + 1;
		x = x >> 1;
	}

	cli();											// mask the interrupts
	outb(inb(INDEX_PORT) | 0x80, INDEX_PORT);		// disable Non-maskable interrupts
	
	outb(STATUS_A, INDEX_PORT);						// set this index to register A
	char prev = inb(DATA_PORT);						// get the initial value of register A
	outb(STATUS_A, INDEX_PORT);						// reset index to A
	
	outb((prev & 0xF0) | rate, DATA_PORT);			// write only our rate to A, rate is the bottom 4 bits

	outb(inb(INDEX_PORT) & 0x7F, INDEX_PORT);		// enable Non-maskable interrupts
	sti();											// unmask the interrupts
	return 0;
}



/*
 * rtc_read()
 *
 * Should always return 0, but only after an interrupt has occurred 
 * (set a flag and wait until the interrupt handler clears it, then 
 * return 0).
 *
 * Inputs: none
 * Retvals: none
 */
uint32_t rtc_read(void) 
{
	int i = 0;
	//while (interrupt_flag == 0)						//keep waiting before interupt happens
	while(1)
	{
		//do nothing
		printf("hello %d" ,i);
		i++;
	}

	interrupt_flag = 0;								//clear flag and return 0

	return 0;
}


/*
 * rtc_write()
 *
 * Should always accept only a 4-byte integer specifying the interrupt 
 * rate in Hz, and should set the rate of periodic interrupts accordingly.
 *
 * Inputs: 
 * set_freq: hz to be set
 * nbytes: number of bytes to set
 * Retvals
 * -1: failure
 * n: number of bytes written
 */

int32_t rtc_write(int32_t * set_freq, int32_t nbytes)
{
	int32_t freq;

	if (nbytes != 4 || set_freq == NULL)			//checking condition
	{
		return -1;
	}

	else
	{
		freq = *set_freq;							
	}

	return setFreq(freq);							//return 0 - all bytes written
}													//return -1 if freq fail limit test in setFreq




/*
 * rtc_open()
 *
 * Opens the RTC.
 *
 * Inputs: none
 * Retvals: 0
 */

uint32_t rtc_open(void)
{
	return 0;
}

/*
 * rtc_close()
 *
 * Closes the RTC.
 *
 * Inputs: none
 * Retvals: 0 
 */
uint32_t rtc_close(void)
{
	return 0;
}
/*
This helper function return a flag indicate if the given number is power of 2.
Taken from http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
*/
int isPowerOfTwo (int32_t x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}


