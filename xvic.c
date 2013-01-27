#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char unk_1[16];
	int checksum;
	unsigned char unk_2[12];
	unsigned char key[32];
} FWHeader;

void decrypt(char *inPath, char *outPath) {
	//Set up our data
	FWHeader header;
	unsigned char hardKey[] = "sinowealth chenshaofan 20121101    ";
	unsigned char encryptionKey[32];
	unsigned char data[0x10000];
	int i, j;
	//Open our input file
	FILE *fp = fopen(inPath, "r");

	//Read the header
	if (fread(&header, 1, 64, fp) != 64) {
		printf("Error reading header\n");
		return;
	}

	//Copy our encoded key into encryptionKey for decoding
	memcpy(&encryptionKey, &header.key, 32);
	
	//Decode the encryption key
	for (i = 0; i < 32; i++) {
		j = i & 0xf;
		encryptionKey[i] = (encryptionKey[i] - hardKey[j + 16]) ^ hardKey[j];
	}

	//Read the rest of the data
	int size = fread(&data, 1, 0x10000, fp);

	//Decrypt it with our key
	for (i = 0; i < size; i++) {
		j = i & 0xF;
		data[i] = encryptionKey[j] ^ (data[i] - encryptionKey[j + 16]);
	}
	fclose(fp);

	//Open the output file
	fp = fopen(outPath, "w");
	//fwrite(&header, 1, sizeof(header), fp);
	fwrite(&data, 1, size, fp);

	fclose(fp);

}

int main(int argc, char **args) {
	decrypt(args[1], args[2]);
	return 0;
}

