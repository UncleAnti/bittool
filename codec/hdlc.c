#include "../bittool.h"

void hdlc_encode(FILE * in, FILE * out, const char * opt) {
	int space = has_char(opt, 's');

  int lastfive = 0;
	int bitcount = 0;
	int byte;

  fputs("01111110", out);
	if (space) fputc(' ', out);

	while ((byte = fgetc(in)) != EOF){
	  if(byte == '0' || byte == '1'){
			fputc(byte, out);
			bitcount++;
		}

		if(byte == '1' && ++lastfive == 5){
			fputc('0', out); // Stuff
			bitcount++;
			lastfive = 0;
		}

		if(byte == '0')
			lastfive = 0;

		if (space && (bitcount % 8 == 0))
			fputc(' ', out);
	}
}

void hdlc_decode(FILE * in, FILE * out, const char * opt) {
	UNUSED(opt);
	//int space = has_char(opt, 's');

	uint16_t history = 0;
  int lastfive = 0;
	int byte;
	int bitcounter = 0;

	while ((byte = fgetc(in)) != EOF){
		if(byte == '0'){
			if(lastfive == 5){
				lastfive = 0;
				continue; // skip this bit
			}

			lastfive = 0;

			history <<= 1;
			bitcounter++;
		}

		if(byte == '1'){
			lastfive = 0;

			history <<= 1;
			history |= 1;
			bitcounter++;
		}

		if((history & 0xFF) == 0x7E){
			fputc('F', out);
			history = 0;
			bitcounter = 0;
			continue;
		}

		if(bitcounter == 8){
			fprintf(out, "%02X ", history);
			history = 0;
		}
	}
}

ENGINE_EXPORT(
	ENGINE_ENTRY("hdlc", 'e', hdlc_encode, "Convert to/from HDLC."),
	ENGINE_ENTRY("hdlc", 'd', hdlc_decode, "Convert to/from HDLC.")
);
