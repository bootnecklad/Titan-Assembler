/*
 * File:   assembler.c
 * Author: fc
 * This file is part of fc's assembler
 * Copyright (C) 2011 fc
 * Created on November 5, 2011, 5:17 PM
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 10 // Maximum length of instruction (jmp 0x0000)
#define MAX_LINE 256 // Maximum length of assembly line (including comments)

void removeComment(char line[]);
void readInstruction(FILE* input, FILE* output);

// Instruction struct, contains information about instruction.

typedef struct {
    unsigned char instruction; // holds actual instruction as a byte.
    unsigned short payload; // holds extra data as two bytes.
    unsigned char type; // type of packet: 0=just instruction,
    // 1=instruction+data 2=no instruction
} ins;

char* opcodes[] = {"NOP", "ADD", "SUB", "AND", "OR", "NOT", "XOR", "PSH",
    "POP", "JMP", "JPI", "JPZ", "JPC", "JPS", "STM", "LDM", "MOV", "CLR"};

unsigned char opcodeValues[] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60,
    0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x70, 0x70};

/* Where all the business goes down. decodes instruction from line
 * builds and returns two instruction structs.
 */
void decode(char* line, ins* bytes) {
    int length = strlen(line);

    bytes[0].type = 0; //default instruction just contains an instruction
    bytes[1].type = 2; //default 2nd instruction is no instruction

    if (strncmp(line, "0x", 2) == 0) { // checks if line contains value.
        char newline[1]; // stores hex value
        unsigned char value;
        strncpy(newline, line + 2, 2);
        xtoi(newline, &value); //sets value the decimal equivalent of newline

        /*this isn't really the instruction, it's just a value (hacked up)*/
        bytes[0].instruction = value;

    } else {
        unsigned char result;
        char instruction[3]; //stores address hex
        strncpy(instruction, line, 3);
        instruction[3] = '\0';

        //loop grabs instruction value from array and stores which instruction.
        int i, instructionNumber;
        for (i = 0; i < 18; i++) {
            if (!strcmp(instruction, opcodes[i])) {
                instructionNumber = i;
                result = opcodeValues[i];
            }
        }

        unsigned char extra; //temp
        char* temp[1]; // temp register
        char temp2[4]; // temp address
        unsigned short result1; // grabbed address from instruction

        /*case statement appends extra data to special instructions
         * and/or creates additional instructions (MOV,CLR)
         */
        switch (instructionNumber) {
            case 7: // PSH
            case 8: // POP
                xtoi(&line[5], &extra);
                result += extra; //reads value from register and adds to data.
                break;

            case 9: // JMP
            case 10: // JPI
                trimWhitespace(&line[6]);
                xtoi(&line[6], &result1); //grabs address and stores in result1

                bytes[0].type = 1;
                break;

            case 14: // STM
            case 15: // LDM
                trimWhitespace(&line[9]);
                xtoi(&line[9], &result1);

                temp[0] = line[5];
                xtoi(temp, &extra);
                result += extra;

                bytes[0].type = 1;
                break;

            case 16: //MOV (acts like PSH)
                temp[0] = line[5];
                xtoi(temp, &extra);
                result += extra; //reads value from register and adds to data.

                temp[0] = line[8];
                xtoi(temp, &extra);
                // stores POP + register value in 2nd instruction.
                bytes[1].instruction = 0x80 + extra;
                bytes[1].type = 0;
                break;

            case 17: //CLR (acts like PSH 0)
                temp[0] = line[5];

                //read register value, store POP + register in 2nd instruction
                xtoi(temp, &extra);
                bytes[1].instruction = 0x80 + extra;
                bytes[1].type = 0;
                break;
            default:
                break;
        }
        bytes[0].instruction = result;
        bytes[0].payload = result1;
    }
}

/*Converts an hexadecimal sequence to an unsigned short. (I didn't write this)
 */
int xtoi(const char* xs, unsigned short* result) {
    size_t szlen = strlen(xs);
    int i, xv, fact;

    if (szlen > 0) {
        // Converting more than 32bit hexadecimal value?
        if (szlen > 8) return 2; // exit

        // Begin conversion here
        *result = 0;
        fact = 1;

        // Run until no more character to convert
        for (i = szlen - 1; i >= 0; i--) {
            if (isxdigit(*(xs + i))) {
                if (*(xs + i) >= 97) {
                    xv = (*(xs + i) - 97) + 10;
                } else if (*(xs + i) >= 65) {
                    xv = (*(xs + i) - 65) + 10;
                } else {
                    xv = *(xs + i) - 48;
                }
                *result += (xv * fact);
                fact *= 16;
            } else {
                // Conversion was abnormally terminated
                // by non hexadecimal digit, hence
                // returning only the converted with
                // an error value 4 (illegal hex character)
                return 4;
            }
        }
    }
    // Nothing to convert
    return 1;
}

/* Truncates the string to ignore any comments
 * Manually searches for the first '/' in string (strcspn should do this)
 * and cuts string off at this index.
 */
void removeComment(char rawline[]) {
    int i;
    for (i = 0; i < strlen(rawline); i++) {
        if (rawline[i] == '/' || rawline[i] == '#') {
            break;
        }
    }
    rawline[i] = '\n';
    rawline[i + 1] = '\0';
}

/*Removes leading and trailing whitespace (I didn't write it)*/
void trimWhitespace(char * s) {
    char * p = s;
    int l = strlen(p);

    while (isspace(p[l - 1])) p[--l] = 0;
    while (* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}

/*Takes whole string and makes characters capital if necessary
 * Ensures all instructions are in uppercase
 */
void toUppercase(char* s) {
    int i;
    for (i = 0; i < strlen(s); i++) {
        s[i] = toupper(s[i]);
    }
}

/* Reads input, removes comments, removes whitespace, puts instruction
 * into uppercase and passes to decode method.
 * (Basically gets and prepares lines to be decoded)
 */
void readInstructions(FILE* in, FILE* out) {
    char rawline[MAX_LINE];

    while (fgets(rawline, MAX_LINE, in) != NULL) {
        trimWhitespace(rawline);
        removeComment(rawline);
        if (rawline[0] != '\n' && strlen(rawline) > 0) {
            toUppercase(rawline);

            ins herp[2];
            decode(rawline, &herp[0]);
            writeBytes(out, &herp[0]);
            fflush(out);
        }
    }
}

/*Writes instruction struct to output file, one byte at a time.
 */
void writeBytes(FILE* output, ins* herp) {
    if (herp[1].type != 2) {
        printf("%d %d\n", herp[0].instruction, herp[1].instruction);
        fwrite(&(herp[0].instruction), 1, 1, output);
        fwrite(&(herp[1].instruction), 1, 1, output);
    } else {

        if (herp[0].type == 1) {
            printf("%d %d\n", herp[0].instruction, herp[0].payload);
            fwrite(&(herp[0].instruction), 1, 1, output);

            union u_type //Setup a Union 
            {
                unsigned short IntVar;
                unsigned char Bytes[2];
            }
            temp; // create temp union to get bytes from short.
            temp.IntVar = herp[0].payload;
            
            fwrite(&(temp.Bytes[1]), 1, 1, output);
            fwrite(&(temp.Bytes[0]), 1, 1, output);
        } else {
            printf("%d \n", herp[0].instruction);
            fwrite(&(herp[0].instruction), 1, 1, output);
        }

    }
}

int main(int argc, char** argv) {
    FILE* in; // File that assembly is read from
    FILE* out; // File that titan machine code is outputted to.

    // Reads console parameters and opens up file streams
    if (argc == 2 || argc == 3) {
        in = fopen(argv[1], "r");

        if (argc == 3) {
            out = fopen(argv[2], "w");
        } else {
            out = fopen("a.out", "w");
        }
        readInstructions(in, out);
        fclose(in);
        fclose(out);

    } else {
        printf("Usage: %s input-file\n", argv[0]);
        printf("Usage: %s input-file output-file\n", argv[0]);
    }

    return (EXIT_SUCCESS);
}