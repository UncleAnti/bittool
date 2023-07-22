#include "../bittool.h"

void bin_encode(FILE * in, FILE * out, const char * opt){
	int space = has_char(opt, 's');
	uint8_t byte;

	while (fread(&byte, sizeof(byte), 1, in) > 0){
		for (size_t bit = 0; bit < 8; ++bit){
			uint8_t val = (byte & 0x80) != 0;
			fputc(val ? '1' : '0', out);
			byte <<= 1;
		}

		if (space)
			fputc(' ', out);
	}

	if (space)
		fputc('\n', out);
}

void bin_decode(FILE *in, FILE *out, const char * opt){
	UNUSED(opt);
	int bitcount = 0;
	uint8_t byte = 0;
	uint8_t bit;

	while (fread(&bit, sizeof(bit), 1, in) > 0){
		if (bit == '1'){
			byte <<= 1;
			byte |= 1;
			++bitcount;
		}

		if (bit == '0'){
			byte <<= 1;
			++bitcount;
		}

		if(bitcount == 8) {
			fwrite(&byte, sizeof(byte), 1, out);
			bitcount = 0;
		}
	}

	if(bitcount > 0) {
		fwrite(&byte, sizeof(byte), 1, out);
		bitcount = 0;
	}
}

ENGINE_EXPORT(
	ENGINE_ENTRY("bin", 'e', bin_encode, "Convert to/from Binary."),
	ENGINE_ENTRY("bin", 'd', bin_decode, "Convert to/from Binary.")
);