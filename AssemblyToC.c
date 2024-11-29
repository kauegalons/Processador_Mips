#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definição dos registradores MIPS
const char *mips_registers[32] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra"
};

// Valores dos registradores
int mips_registers_vals[32] = { 0 }; 

// Memória e Program Counter
int memory[1024] = { 0 }; 
int PC = 0; 

// Função para encontrar índice do registrador
int find_register_index(char *input) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(input, mips_registers[i]) == 0) {
            return i;
        }
    }
    return -1; 
}

// Funções de operações (Tipo R)
void ADD(char *regSave, char *regOp1, char *regOp2) {
    int regOp1Index = find_register_index(regOp1);
    int regOp2Index = find_register_index(regOp2);
    int regSaveIndex = find_register_index(regSave);
    mips_registers_vals[regSaveIndex] = mips_registers_vals[regOp1Index] + mips_registers_vals[regOp2Index];
    printf("ADD: %s = %d + %d\n", regSave, mips_registers_vals[regOp1Index], mips_registers_vals[regOp2Index]);
}

void SUB(char *regSave, char *regOp1, char *regOp2) {
    int regOp1Index = find_register_index(regOp1);
    int regOp2Index = find_register_index(regOp2);
    int regSaveIndex = find_register_index(regSave);
    mips_registers_vals[regSaveIndex] = mips_registers_vals[regOp1Index] - mips_registers_vals[regOp2Index];
    printf("SUB: %s = %d - %d\n", regSave, mips_registers_vals[regOp1Index], mips_registers_vals[regOp2Index]);
}

void AND(char *regSave, char *regOp1, char *regOp2) {
    int regOp1Index = find_register_index(regOp1);
    int regOp2Index = find_register_index(regOp2);
    int regSaveIndex = find_register_index(regSave);
    mips_registers_vals[regSaveIndex] = mips_registers_vals[regOp1Index] & mips_registers_vals[regOp2Index];
    printf("AND: %s = %d & %d\n", regSave, mips_registers_vals[regOp1Index], mips_registers_vals[regOp2Index]);
}

void JR(char *reg) {
    int regIndex = find_register_index(reg);
    if (regIndex < 0) {
        printf("Erro: Registrador inválido para JR.\n");
        return;
    }
    printf("JR: PC alterado de %d para %d\n", PC, mips_registers_vals[regIndex]);
    PC = mips_registers_vals[regIndex];
}

// Funções de operações (Tipo I)
void ADDI(char *regSave, char *regOp, int immediate) {
    int regOpIndex = find_register_index(regOp);
    int regSaveIndex = find_register_index(regSave);
    mips_registers_vals[regSaveIndex] = mips_registers_vals[regOpIndex] + immediate;
    printf("ADDI: %s = %d + %d\n", regSave, mips_registers_vals[regOpIndex], immediate);
}

void LW(char *regSave, char *regBase, int offset) {
    int regSaveIndex = find_register_index(regSave);
    int regBaseIndex = find_register_index(regBase);

    int address = mips_registers_vals[regBaseIndex] + offset;
    if (address % 4 != 0 || address / 4 >= 1024 || address < 0) {
        printf("Erro: endereço fora dos limites da memória.\n");
        return;
    }

    mips_registers_vals[regSaveIndex] = memory[address / 4];
    printf("LW: Carregado valor %d de memory[%d] para %s\n", mips_registers_vals[regSaveIndex], address / 4, regSave);
}

void BEQ(char *reg1, char *reg2, int offset) {
    int reg1Index = find_register_index(reg1);
    int reg2Index = find_register_index(reg2);
    if (mips_registers_vals[reg1Index] == mips_registers_vals[reg2Index]) {
        PC += offset;
        printf("BEQ: Branch taken, PC = %d\n", PC);
    } else {
        printf("BEQ: Branch not taken\n");
    }
}

// Funções de operações (Tipo J)
void J(int address) {
    printf("JUMP: PC alterado de %d para %d\n", PC, address);
    PC = address;
}

void JAL(int address) {
    mips_registers_vals[31] = PC + 1; 
    printf("JAL: $ra = %d, PC alterado para %d\n", mips_registers_vals[31], address);
    PC = address;
}



// Função para imprimir valores dos registradores
void print_registers() {
    for (int i = 0; i < 32; i++) {
        printf("Register %s: %d\n", mips_registers[i], mips_registers_vals[i]);
    }
    printf("PC: %d\n", PC);
    printf("-------------------------------------------------\n");
}


// Função para tratar entrada do usuário
void process_instruction(char *input) {
    char operation[10], reg1[10], reg2[10], reg3[10];
    int immediate = 0;

    // Formato esperado de entrada: "ADD $t0 $t1 $t2"
    sscanf(input, "%s %s %s %s", operation, reg1, reg2, reg3);
    immediate = atoi(reg3);

    printf("Formato Estrutural da Instrução:\n");
    if (strcmp(operation, "ADD") == 0) {
        printf("Opcode: 0\nrs: %d (%s)\nrt: %d (%s)\nrd: %d (%s)\nshamt: 0\nfunct: 32 (ADD)\n", 
               find_register_index(reg2), reg2, find_register_index(reg3), reg3, find_register_index(reg1), reg1);
        ADD(reg1, reg2, reg3);
    } else if (strcmp(operation, "SUB") == 0) {
        printf("Opcode: 0\nrs: %d (%s)\nrt: %d (%s)\nrd: %d (%s)\nshamt: 0\nfunct: 34 (SUB)\n", 
               find_register_index(reg2), reg2, find_register_index(reg3), reg3, find_register_index(reg1), reg1);
        SUB(reg1, reg2, reg3);
    } else if (strcmp(operation, "AND") == 0) {
        printf("Opcode: 0\nrs: %d (%s)\nrt: %d (%s)\nrd: %d (%s)\nshamt: 0\nfunct: 36 (AND)\n", 
               find_register_index(reg2), reg2, find_register_index(reg3), reg3, find_register_index(reg1), reg1);
        AND(reg1, reg2, reg3);
    } else if (strcmp(operation, "ADDI") == 0) {
        printf("Opcode: 8 (ADDI)\nrs: %d (%s)\nrt: %d (%s)\nimmediate: %d\n", 
               find_register_index(reg2), reg2, find_register_index(reg1), reg1, immediate);
        ADDI(reg1, reg2, immediate);
    } else if (strcmp(operation, "LW") == 0) {
        int offset = immediate;
        printf("Opcode: 35 (LW)\nrs: %d (%s)\nrt: %d (%s)\noffset: %d\n", 
               find_register_index(reg2), reg2, find_register_index(reg1), reg1, offset);
        LW(reg1, reg2, offset);
    } else if (strcmp(operation, "BEQ") == 0) {
        int offset = immediate;
        BEQ(reg1, reg2, offset);
    } else if (strcmp(operation, "J") == 0) {
        int address = atoi(reg1);
        J(address);
    } else if (strcmp(operation, "JAL") == 0) {
        int address = atoi(reg1);
        JAL(address);
    } else if (strcmp(operation, "JR") == 0) {
        JR(reg1);
    }
    print_registers();
}

int main() {
    char buffer[100];

    // Inicializa valores para testes
    mips_registers_vals[8] = 10; // $t0
    mips_registers_vals[9] = 10; // $t1
    mips_registers_vals[10] = 30; // $t2
    mips_registers_vals[4] = 100; // $a0 (registrador base)

    // Inicializa a memória no endereço 104 com valor 50
    memory[104 / 4] = 50; // Como a memória é endereçada por palavras (4 bytes), usamos 104 / 4



    printf("Digite uma instrução MIPS ou 'sair' para encerrar:\n");
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);

        // Remove o newline (\n) da entrada
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "sair") == 0) {
            break; // Encerra o loop se o usuário digitar 'sair'
        }

        process_instruction(buffer);
        printf("Digite uma instrução MIPS ou 'sair' para encerrar:\n");

    }

    return 0;
}