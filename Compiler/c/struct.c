#include <assert.h>
#include <malloc.h>
#include <stdio.h>

static void loadContents(void);
static void printContents(void);

int main(int argc, char* argv[]) {
  loadContents();
  printContents();
  return (0);
}

typedef struct CharCell {
  char _character;
  struct CharCell* _next;
} CharCell;

typedef struct LineCell {
  CharCell* _head;
  struct LineCell* _next;
} LineCell;

static LineCell* contentsHead = NULL;

static LineCell* loadLine(void);

static void loadContents(void) {
  LineCell* previousLine = NULL;
  LineCell* currentLine;
  while (currentLine = loadLine()) {
    if (previousLine == NULL) {
      contentsHead = currentLine;
    } else {
      previousLine->_next = currentLine;
    }
    previousLine = currentLine;
  }
}

static CharCell* allocateCharCell(char character);
static LineCell* allocateLineCell();

static LineCell* loadLine(void) {
  int c = getchar();

  if (c == EOF) {
    return (NULL);
  }

  {
    LineCell* line = allocateLineCell();
    CharCell* previousChar;
    for (previousChar = NULL; c != '\n'; c = getchar()) {
      assert(c != EOF);
      {
        CharCell* currentChar = allocateCharCell((char)c);
        if (previousChar == NULL) {
          line->_head = currentChar;
        } else {
          previousChar->_next = currentChar;
        }
        previousChar = currentChar;
      }
    }

    return (line);
  }
}

static CharCell* allocateCharCell(char character) {
  CharCell* result = (CharCell*)malloc(sizeof(CharCell));
  result->_character = character;
  result->_next = NULL;
  return (result);
}

static LineCell* allocateLineCell() {
  LineCell* result = (LineCell*)malloc(sizeof(LineCell));
  result->_head = NULL;
  result->_next = NULL;
  return (result);
}

static void printLine(LineCell* line);

static void printContents(void) {
  LineCell* currentLine = contentsHead;
  for (; currentLine; currentLine = currentLine->_next) {
    printLine(currentLine);
  }
}

static void printLine(LineCell* line) {
  CharCell* currentChar = line->_head;
  for (; currentChar; currentChar = currentChar->_next) {
    printf("%c", currentChar->_character);
  }
  printf("\n");
}
