

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// TODO if it puts 0 in the packet instead of 0000.... the comparaison would be rip


//Compare all the different packets of the same lane... (example all the packets with 0x0020)
/*The input must be :
0x0020: 7e5a e17b 6a00 0154 015f 2300 00a9 8ffe (line 2 of a packet)
0x0020: 7e5a 02e6 7b6a 0000 e77b 6a00 0154 0153 (line 2 of another packet)
0x0020: 7e5a 02f2 7b6a 0001 5401 fc1e 0000 4886 (line 2 of another packet)
Which means it will compare all the second lanes, and will tell you which byte is never changed in all the packets
*/

// Next step : compare all the packets directly, don't do just one line of a pcket



#define LENGTH 5000 // length of a line , way too high but it's just in case
#define LINENUMBERMAX 500 //Maximum number of lines in a packets
/*For example a packet read from tcpdump would be like this:
0x0010:  2e69 1f52 e4c8 8595 0035 1954 0602 c2a3
0x0020:  7f5a 02e3 646d 0001 5401 c5fd ffff 1c7a
0x0030:  feff 2a0b 0000 9d80 feff 0000 0000 3d00
0x0040:  e464 6d00 00da 646d 00

The number of lines must be lower than LINEMAX, the length of a line should be lower than SIZE
*/
int main() 
{
	FILE *file;
	char *line;
	char data[LENGTH];
	char start[LENGTH];
	char buff[LENGTH];
	int i =0;
	int j =0;
	char refpacket[LENGTH][LINENUMBERMAX]; // the first packet read in the file
	char otherpacket[LENGTH][LINENUMBERMAX]; // the other packet which would be compared to refpacket
	char diffligne[LENGTH][LINENUMBERMAX]; // 3 if same on all the different packets, 0 if it's different at least once, -1 if never compared before
	int linenumber = 0; // The number of the line, if the line has 0x0000, it will be 0. if it's 0x0020, it will be 2
	int packetnumber = -1; // The number of the packet,so if it's the packet number 2, it will be stock in refpacket[LENGTH][2] // -1  because when the loop starts, it does +1. 
	int firstpacketread = 0; // once the first packet is read and stocked in refpacket, only add elements in other tabs
	int debug = 1; // activate debug mode

	for(i=0;i<LINENUMBERMAX;i++){
		for(j=0;j<LENGTH;j++){
			refpacket[j][i] = '\0';
			diffligne[j][i]=-1; // -1 for meaning : not used yet

		}
	}
	i=0;j=0;

	
	printf("start:\n");
	file = fopen("test3.txt", "r");
	if(file==NULL){
		perror("file not found");
		return 0;
	}



	if (file != NULL) {
		while (fgets (buff, LENGTH, file)) {
			if(strstr(buff,"0x00") == NULL)
				continue; // line not interesting, skip it

			sscanf(buff,"%[^:]:%[^\n][\n]",start,data); // stock the line in 2 variable : the first one will have the 0x0010 for example, the other will have the data

			// start the work on the line :
			if(strstr(start, "0x0000") != NULL){
				linenumber=0; packetnumber++;  //reset if new packet
			}


			for(i=0;i<LENGTH;i++){
				if (packetnumber==0)
					refpacket[i][linenumber] = data[i];
				else
					otherpacket[i][linenumber] = data[i];
				if (data[i] == '\0')
					break;
			}

			for(i=0;i<LENGTH;i++){  // compare the whole lane
				if (data[i] == '\0' || refpacket[i][linenumber] == '\0' || otherpacket[i][linenumber] == '\0') break; // end of a lane or packet, don't compare
				// INFO : what if you compare 2 packets of different length...in this case we don't compare

				if(diffligne[i][linenumber] != 0){ // It was never  compared before or it was compared but same value, so compare it again
					if(refpacket[i][linenumber] == otherpacket[i][linenumber]) // same byte
						diffligne[i][linenumber]=3;
					else // different byte, put 0 and never compare this byte again since it must be always the same for every packet
						diffligne[i][linenumber]=0;
				}
			}

			linenumber++; // end of the line, increase it
    		}
	}

	int t=0;
	int f=0;
	int leaveloop=0;
	printf("print reference packet\n");
	for(t=0;t<LINENUMBERMAX;t++){
		for(f=0;f<LENGTH;f++){
			putchar(refpacket[f][t]);
			if(refpacket[0][t] == '\0')
				leaveloop=1;
			if(refpacket[f][t] == '\0')
				 break;
		}
		printf("\n");
		if(leaveloop==1)
			break;
	}
	leaveloop=0;
	printf("print other\n");
	for(t=0;t<LINENUMBERMAX;t++){
		for(f=0;f<LENGTH;f++){

			//doesn't print stuff other than udp data
			if(otherpacket[0][t] == '\0')
				leaveloop=1;
			if(otherpacket[f][t] == '\0')
				 break;
			if(t <2)
				if(t == 1 && f < 32 || t == 0){
				if(otherpacket[f][t]== ' ')
					printf(" ");
				else
					printf("x");
				continue; 
			}
			putchar(otherpacket[f][t]);

		}
		printf("\n");
		if(leaveloop==1)
			break;
	}
	leaveloop=0;
	printf("print comparison : Highlight\n");
	for(t=0;t<LINENUMBERMAX;t++){
		for(f=0;f<LENGTH;f++){
			if(otherpacket[0][t] == '\0')
				leaveloop=1;
			if(otherpacket[f][t] == '\0')
				 break;
			//doesn't print stuff other than udp data
			if(t<2)
				if(t == 1 && f < 32 || t == 0){
				if(otherpacket[f][t]== ' ')
					printf(" ");
				else
					printf("x");
				continue; 
			}

			if(otherpacket[f][t] == ' ')
				putchar(otherpacket[f][t]);
			else if(diffligne[f][t] == 3){
				printf(">");putchar(otherpacket[f][t]);printf("<");
			}
			else
				putchar(otherpacket[f][t]);
		}
		printf("\n");
		if(leaveloop==1)
			break;
	}
	leaveloop=0;
	printf("Bytes location differences(3 for same in all the packets, 0 for different\n");
	for(t=0;t<LINENUMBERMAX;t++){
		for(f=0;f<LENGTH;f++){
			if(otherpacket[0][t] == '\0')
				leaveloop=1;
			if(otherpacket[f][t] == '\0')
				 break;
			//doesn't print stuff other than udp data
			if(t<2)
				if(t == 1 && f < 32 || t == 0){
				if(otherpacket[f][t]== ' ')
					printf(" ");
				else
					printf("x");
				continue; 
			}


			if(otherpacket[f][t] == ' ')
				printf(" ");
			else
				printf("%i",diffligne[f][t]);
		}
		printf("\n");
		if(leaveloop==1)
			break;
	}

	fclose(file);
	return 1;
}
