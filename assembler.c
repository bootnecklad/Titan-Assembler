/* This file is part of bootnecklad's assembler for the homebrew processor TITAN
   Copyright (C) 2011 bootnecklad, bootnecklad@gmail.com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by  
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License  
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAXLEN 100 /* max number of characters per line */

void parse(char *ins, char *line, char *operand)
{
   if(strncmp(line,"/",1) == FALSE)
   {
      //printf("Comment\n");
   }
   else if(strncmp(line,"0",1) == FALSE)
   {
      /* Seperates data from the '0x', puts in format 0.. */
	  ins[0] = '0';
	  ins[1] = line[2];
	  ins[2] = line[3];
	  ins[3] = 0;
   }
   else
   {
      /* Takes out instruction */
	  ins[0] = line[0];
	  ins[1] = line[1];
	  ins[2] = line[2];
	  ins[3] = 0;
   }
}

int main(int argc, char *argv[])
{
   char ins[4];
   char line[MAXLEN];
   char operand[5];
   
   /* checks for correct command line input */
   if(argc != 3)
   {
      printf("Invalid input\n");
	  return 0;
   }   
   
   /* opens files for reading and writin, defined by input */
   FILE* input = fopen(argv[1],"r");
   FILE* output = fopen(argv[2],"w");
   
   while(fgets(line, MAXLEN, input))
   {
      parse(ins, line, operand);
	  //fprintf(output, "%s", ins);
	  printf("%s\n", ins);
   }
   
   fclose(input);
   fclose(output);
   
   return(0);
}