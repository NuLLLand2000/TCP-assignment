#include <stdio.h>
#include <openssl/md5.h>


unsigned char* getMD5(char *input_string) {
	unsigned static char c[MD5_DIGEST_LENGTH];
    //char *filename="md5.c";
	char *filename = input_string;
    int i;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0) {
        MD5_Update (&mdContext, data, bytes);
    }
    
    MD5_Final (c,&mdContext);
    
    for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
    	printf("%02x", c[i]);
    }
    
    printf (" %s\n", filename);
    fclose (inFile);
    return c;
}

int main(int argc, char *argv[]) {
	char *file_name = argv[1];
	printf("%s", getMD5(file_name));
    return 0;
}
