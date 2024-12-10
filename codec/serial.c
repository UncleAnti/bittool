#include "../bittool.h"

void serial_encode(FILE * in, FILE * out, const char * opt) {
	int space = has_char(opt, 's');

	int bitcount = 0;
	int byte = 0;
	int parity = 0;
  char buf[8];

	while ((byte = fgetc(in)) != EOF){
	  if(byte == '0' || byte == '1'){
	    if(byte == '1')
	      parity++;

      buf[bitcount] = byte;
			bitcount++;
		}

		if (bitcount == 8){
      fputs("0", out); // Start
      for(size_t idx=0; idx < 8; ++idx)
        fputc(buf[7 - idx], out);

      //if((parity & 1)) fputc('0', out); // Parity - Even
      //if((parity & 1)) fputc('1', out); // Parity - Odd

      // Stop Bit
      fputc('1', out);

		  if(space)
        fputc(' ', out);

      parity = 0;
		}
	}
}

void serial_decode(FILE * in, FILE * out, const char * opt) {
	UNUSED(opt);

	int bitcount = 0;
	int byte = 0;
	int parity = 0;
	int start = 0;
  char buf[10];

	while ((byte = fgetc(in)) != EOF){
	  if(!start && byte == '0'){
      start = 1;
      bitcount = 0;
      byte = 0;
      parity = 0;
      continue;
	  }

	  if(start && (byte == '0' || byte == '1')){
      buf[bitcount++] = byte;
      if(byte == '1') parity++;
    }

		if(bitcount > 8){
		  // Ignore Parity
		  if(byte == '1'){ // Valid Stop
		    for(size_t idx=0;idx<8; ++idx)
			    fputc(buf[7 - idx], out);

			  start = 0;
			  bitcount = 0;
			  parity = 0;
			} else {
			  start = 0;
			  fputc('*', out);
			}
		}
	}
}

ENGINE_EXPORT(
	ENGINE_ENTRY("serial", 'e', serial_encode, "Convert to serial."),
	ENGINE_ENTRY("serial", 'd', serial_decode, "Convert from serial.")
);
