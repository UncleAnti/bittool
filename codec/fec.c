#include "../bittool.h"
#include <fec.h>
#include <stdlib.h>
#include <string.h>

const int symsize = 3;
const int nroots = 3;
const int block_size = (1 << symsize) - 1;
const int data_size = block_size - nroots;

void fec_encode(FILE *in, FILE *out, const char *opt)
{
	int space = has_char(opt, 's');

	void *rs = init_rs_char(symsize, 0x13, 1, 1, nroots, 0);
	if (rs == NULL)
	{
		fprintf(stderr, "init_rs_char failed!\n");
		return;
	}
	//else fprintf(stderr, "Initalized RS %i/%i\n", 1 << data_size, 1 << (block_size - data_size));

	unsigned char *block = (unsigned char *)malloc(block_size);
	uint8_t byte;
	uint8_t data = 0;
	uint8_t bitc = 0;
	uint8_t idx = 0;
	while (fread(&byte, sizeof(byte), 1, in) > 0){
		if (byte == '1'){
			data <<= 1;
			data |= 1;
			bitc++;
		}

		if (byte == '0'){
			data <<= 1;
			bitc++;
		}

		if (bitc == symsize){
			block[idx++] = data;
			bitc = 0;
			data = 0;
		}

		if (idx == data_size)
		{
			encode_rs_char(rs, block, block + data_size);

			const uint8_t bit = (1 << (symsize - 1));
			for (int i = 0; i < block_size; ++i)
			{
				for (int b = 0; b < symsize; ++b)
				{
					if (block[i] & bit)
						fputc('1', out);
					else
						fputc('0', out);
					block[i] <<= 1;
				}

				if (space)
					fputc(' ', out);
			}
			if (space)
				fputc('\n', out);

			memset(block, 0, block_size);
			idx = 0;
		}
	}
	if (idx != 0) {
		fprintf(stderr, "underrun by %i bits\n", (idx * symsize)+bitc);
	}
	free(block);
	fflush(out);
	free_rs_char(rs);
}

void fec_decode(FILE *in, FILE *out, const char *opt)
{
	int space = has_char(opt, 's');

	void *rs = init_rs_char(symsize, 0x13, 1, 1, nroots, 0);
	if (rs == NULL)
	{
		fprintf(stderr, "init_rs_char failed!\n");
		return;
	} 
	//else fprintf(stderr, "Initalized RS %i/%i\n", 1 << data_size, 1 << (block_size - data_size));

	unsigned char *block = (unsigned char *)malloc(block_size);
	uint8_t byte;
	uint8_t data = 0;
	uint8_t bitc = 0;
	uint8_t idx = 0;
	while (fread(&byte, sizeof(byte), 1, in) > 0)
	{
		if (byte == '1'){
			data <<= 1;
			data |= 1;
			bitc++;
		}

		if (byte == '0'){
			data <<= 1;
			bitc++;
		}

		if (bitc == symsize){
			block[idx++] = data;
			bitc = 0;
			data = 0;
		}

		if (idx == block_size){
			int epos[nroots-1];
			int ret = decode_rs_char(rs, block, epos, 0);
			
			if (ret > 0){
				fprintf(stderr, "Err[%i", epos[0]);
				for(int i=1; i<ret; ++i)
					fprintf(stderr, ",%i", epos[i]);
				fprintf(stderr, "]\n");
			} else if (ret == 0){
				const uint8_t bit = (1 << (symsize - 1));
				for (int i = 0; i < data_size; ++i){
					for (int b = 0; b < symsize; ++b){
						if (block[i] & bit)
							fputc('1', out);
						else
							fputc('0', out);
						block[i] <<= 1;
					}

					if (space)
						fputc(' ', out);
				}
				
				if (space)
					fputc('\n', out);

				memset(block, 0, block_size);
				idx = 0;
			} else
				fprintf(stderr, "BadFec\n");
		}	
	}
	if (idx != 0) {
		fprintf(stderr, "underrun by %i bits\n", (idx * symsize)+bitc);
	}

	free(block);

	free_rs_char(rs);
}

ENGINE_EXPORT(
		ENGINE_ENTRY("fec", 'e', fec_encode, "Convert to/from forward-error-correction."),
		ENGINE_ENTRY("fec", 'd', fec_decode, "Convert to/from forward-error-correction."),
);
