#include "../bittool.h"
#include <fec.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const int symsize = 4;
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
	}// else fprintf(stderr, "Initalized RS Block = %i, Data = %i, FEC = %i\n", block_size * symsize, data_size * symsize, (block_size - data_size) * symsize);

	unsigned char *block = (unsigned char *)malloc(block_size);
	uint8_t byte;
	uint8_t data = 0;
	uint8_t bitc = 0;
	uint8_t idx = 0;
	bool eof = false;
	while(1){
    if(eof)
      byte = '0';

	  if(!eof && fread(&byte, sizeof(byte), 1, in) != 1){
      eof = true;
      if(idx == 0 && bitc == 0) // Dont start a new block
        break;
    }

		if (byte == '1'){
			data <<= 1;
			data |= 1;
			bitc++;
		} else if (byte == '0'){
			data <<= 1;
			bitc++;
		} else continue;

    //fprintf(stderr, "%i.%i = %i\n", idx,bitc,byte);

		if (bitc == symsize){
			block[idx++] = data;
			bitc = 0;
			data = 0;

  		if (idx == data_size){
	  	  //fprintf(stderr, "Absorbed %i bits\n", (idx * symsize) + bitc);
		  	encode_rs_char(rs, block, block + data_size);

  			const uint8_t bit = (1 << (symsize - 1));
	  		for (int i = 0; i < block_size; ++i){
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
	  		if(eof)
	  		  break;
  		}
  	}
	}

	free(block);
	fflush(out);
	free_rs_char(rs);
}

void fec_decode(FILE *in, FILE *out, const char *opt)
{
	int space = has_char(opt, 's');

	void *rs = init_rs_char(symsize, 0x13, 1, 1, nroots, 0);
	if (rs == NULL){
		fprintf(stderr, "init_rs_char failed!\n");
		return;
	}// else fprintf(stderr, "Initalized RS Block = %i, Data = %i, FEC = %i\n", block_size * symsize, data_size * symsize, (block_size - data_size) * symsize);

	unsigned char *block = (unsigned char *)malloc(block_size);
	uint8_t byte;
	uint8_t data = 0;
	uint8_t bitc = 0;
	uint8_t idx = 0;
	bool eof = false;
	while (1)
	{
    if(eof)
      byte = '0';

	  if(!eof && fread(&byte, sizeof(byte), 1, in) != 1){
      eof = true;
      if(idx == 0 && bitc == 0) // Skip new block
        break;
    }

		if (byte == '1'){
			data <<= 1;
			data |= 1;
			bitc++;
		} else if (byte == '0'){
			data <<= 1;
			bitc++;
		} else continue;

		if (bitc == symsize){
			block[idx++] = data;
			bitc = 0;
			data = 0;
		}

		if (idx == block_size){
			int epos[nroots-1];
			int ret = decode_rs_char(rs, block, epos, 0);

			if (ret > 0){
				//fprintf(stderr, "E@[%i", epos[0]);
				//for(int i=1; i<ret; ++i) fprintf(stderr, ",%i", epos[i]);
				//fprintf(stderr, "]");
			}

			if(ret < 0){
        fprintf(stderr, "Bad FEC, returned %i\n", ret);
      }

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
      if(eof)
        break;
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
