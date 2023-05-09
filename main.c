#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 131072

// será uma lista simplesmente encadeada.
typedef struct commands {
  char command[6];
  int number;  //-1 se não tiver.
  struct commands *next;
} Commands;

// será armazenado no BCP
typedef struct process {
  char name[100];
  int segment_id;
  int priority;
  int segment_size;
  char semaphores[100];

  Commands *commads_head;
} Process;

// será uma lista simplesmente encadeada.
typedef struct bcp {
  Process process;
  int process_state;
  struct bcp *next, *prev;
} BCP;

// será um vetor na memória.
typedef struct block {
  int segment_id;             // se for -1, então está livre.
  int next_segment_location;  // se for -1, então é o último.
} Block;

typedef struct memory {
  Block blocks[MEMORY_SIZE];  // blocos com 8kb.
  int current_occupation;     // porcentagem.
} Memory;

Memory memory;
BCP *bcp_head = NULL, *bcp_tail = NULL;

void reset() {
  int i;
  memory.current_occupation = 0;
  for (i = 0; i < MEMORY_SIZE; i++) {
    memory.blocks[i].segment_id = -1;
    memory.blocks[i].next_segment_location = -1;
  }
}

void processCreate(char file_name[100]) {
  FILE *fp = fopen(file_name, "r");
  BCP *new = malloc(sizeof(BCP));
  Commands *commands = malloc(sizeof(Commands));
  commands->next = NULL;
  new->process.commads_head = commands;

  fscanf(fp, "%s", new->process.name);
  fscanf(fp, "%d\n", &new->process.segment_id);
  fscanf(fp, "%d\n", &new->process.priority);
  fscanf(fp, "%d\n", &new->process.segment_size);
  fscanf(fp, "%[^\n]", new->process.semaphores);

  while (!feof(fp)) {
    fscanf(fp, "%s", commands->command);
    commands->number = -1;
    if (!strcmp(commands->command, "exec") ||
        !strcmp(commands->command, "read") ||
        !strcmp(commands->command, "write") ||
        !strcmp(commands->command, "print")) {
      fscanf(fp, "%d", &commands->number);
    }
    commands->next = NULL;
    if (!feof(fp)) {
      commands->next = malloc(sizeof(Commands));
      commands = commands->next;
    }
  }

  int blocks_occupied, prev_block, i=0, j=0;

  blocks_occupied = new->process.segment_size / 8;
  if (blocks_occupied == 0) blocks_occupied = 1;

  if (MEMORY_SIZE - blocks_occupied < 0) {
    printf("Nao cabe na memoria.\n");
    free(new);
    return;
  }

  printf("Processo %s criado, adicionado aos blocos ", new->process.name);
  for (i = 0; i < 131072 && j < blocks_occupied; i++) {
    if (memory.blocks[i].segment_id == -1) {
      memory.blocks[i].segment_id = new->process.segment_id;
      printf("%d ", i);
      if (j > 0) {
        memory.blocks[prev_block].next_segment_location = i;
      }
      prev_block = i;
      j++;
    }
  }
  printf("da memoria.\n");

  if (bcp_head == NULL) {
    bcp_head = new;
    bcp_tail = new;
    bcp_head->next = NULL;
    return;
  }
  bcp_tail->next = new;
  bcp_tail = new;
  return;
}

int main() {
  reset();
  char file_name[100];
  printf("Nome arquivo: ");
  scanf("%s", file_name);
  processCreate(file_name);
  processCreate(file_name);
  processCreate(file_name);
  processCreate(file_name);
  return 0;
}