#include "../headers/monitor.h"
int cursorX = 0, cursorY = 0;
const uint8 sw = 80,sh = 25,sd = 2;
int color = 0x0F;
void clearLine(uint8 from,uint8 to){
  uint16 i = sw * from * sd;
  //The text screen video memory for colour monitors resides at 0xB8000,
  //http://wiki.osdev.org/Printing_To_Screen
  string mem=(string)0xb8000;
  for(i;i<(sw*to*sd);i++){
    mem[(i / 2)*2 + 1 ] = color ;
    mem[(i / 2)*2 ] = 0;
  }
}
void updateCursor(){
  unsigned temp;
  temp = cursorY * sw + cursorX-1;
  outportb(0x3D4, 14);
  outportb(0x3D5, temp >> 8);
  outportb(0x3D4, 15);
  outportb(0x3D5, temp);
}
void clear(){
  clearLine(0,sh-1);
  cursorX = 0;
  cursorY = 0;
  updateCursor();
}

void scrollUp(uint8 lineNumber){
  string mem = (string)0xb8000;
  uint16 i = 0;
  clearLine(0,lineNumber-1);
  for (i;i<sw*(sh-1)*2;i++)
  {
    mem[i] = mem[i+sw*2*lineNumber];
  }
  clearLine(sh-1-lineNumber,sh-1);
  if((cursorY - lineNumber) < 0 ){
    cursorY = 0;
    cursorX = 0;
  }else{
    cursorY -= lineNumber;
  }
  updateCursor();
}


void newLineCheck(){
  if(cursorY >=sh-1){
    scrollUp(1);
  }
}
void printch(char c){
  string mem = (string) 0xb8000;
  switch(c){
    case (0x08):
      if(cursorX > 0){
        cursorX--;
        mem[(cursorY * sw + cursorX)*sd]=0;
      }
      break;
    case ('\r'):
      cursorX = 0;
      break;
    case ('\n'):
      cursorX = 0;
      cursorY++;
      break;
    default:
      mem [((cursorY * sw + cursorX))*sd] = c;
      mem [((cursorY * sw + cursorX))*sd+1] = color;
      cursorX++;
      break;
  }
  if(cursorX >= sw){
    cursorX = 0;
    cursorY++;
  }
  updateCursor();
  newLineCheck();
}
void print (string ch){
  uint16 i = 0;
  uint8 length = strlen(ch);
  for(i;i<length;i++){
    printch(ch[i]);
  }
}
