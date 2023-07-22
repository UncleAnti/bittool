#include "../bittool.h"

void hdlc_encode(FILE * in, FILE * out, const char * opt) {
	int space = has_char(opt, 's');

  int lastfive = 0;
	int bitcount = 0;
	int byte;

	while ((byte = fgetc(in)) != EOF){
		fputc(byte, out); // Send Bit
		bitcount++;

		if(byte == '1' && ++lastfive == 5){
			fputc('0', out); // Stuff
			bitcount++;
			lastfive = 0;
		}

		if(byte == '0')
			lastfive = 0;

		if (space && (bitcount % 8 == 7))
			fputc(' ', out);
	}
}

void hdlc_decode(FILE * in, FILE * out, const char * opt) {
	UNUSED(opt);
	//int space = has_char(opt, 's');

  int lastfive = 0;
	int byte;

	while ((byte = fgetc(in)) != EOF){
		if (byte == '0' && lastfive == 5){
			lastfive = 0;
			continue;//skip the bitstuff
		}

		if (byte == '1' && lastfive == 5){
			fprintf(stderr, "sync");
			lastfive = 0;
			continue;//skip the bitstuff
		}
		
		if(byte == '1')
			lastfive++;
		
		if(byte == '0')
			lastfive = 0;

		fputc(byte, out); // Send Bit
	}
}

ENGINE_EXPORT(
	ENGINE_ENTRY("hdlc", 'e', hdlc_encode, "Convert to/from HDLC."),
	ENGINE_ENTRY("hdlc", 'd', hdlc_decode, "Convert to/from HDLC.")
);
